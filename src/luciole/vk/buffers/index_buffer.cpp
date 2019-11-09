/**
 *  Copyright (C) 2018-2019 Wmbat
 *
 *  wmbat@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  You should have received a copy of the GNU General Public License
 *  GNU General Public License for more details.
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <luciole/vk/buffers/index_buffer.hpp>

namespace vk
{
   index_buffer::index_buffer( index_buffer::create_info_t const& create_info )
      :
      memory_allocator( create_info.value( ).p_context->get_memory_allocator( ) ),
      allocation( VK_NULL_HANDLE ),
      buffer( VK_NULL_HANDLE )
   {
      auto const buffer_size =
         create_info.value( ).indices.size( ) *
         sizeof( create_info.value( ).indices[0] );

      /* STAGING BUFFER */
      VkBuffer staging_buffer = VK_NULL_HANDLE;
      VmaAllocation staging_memory = VK_NULL_HANDLE; 

      VkBufferCreateInfo const staging_create_info
      {
         .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
         .pNext = nullptr,
         .flags = 0,
         .size = buffer_size,
         .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
         .sharingMode = VK_SHARING_MODE_CONCURRENT,
         .queueFamilyIndexCount = static_cast<std::uint32_t>(
            create_info.value( ).family_indices.size( )
         ),
         .pQueueFamilyIndices = create_info.value( ).family_indices.data( )
      };

      VmaAllocationCreateInfo staging_alloc_info = { };
      staging_alloc_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;

      vmaCreateBuffer(
         memory_allocator,
         &staging_create_info,
         &staging_alloc_info,
         &staging_buffer,
         &staging_memory,
         nullptr
      );

      /* MAP THE MEMORY INTO THE STAGING BUFFER */
      void* data;
      vmaMapMemory( memory_allocator, staging_memory, &data );
      memcpy(
         data,
         create_info.value( ).indices.data( ),
         buffer_size
      );
      vmaUnmapMemory( memory_allocator, staging_memory );

      /* INDEX BUFFER */
      VkBufferCreateInfo const index_create_info
      {
         .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
         .pNext = nullptr,
         .flags = 0,
         .size = buffer_size,
         .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
         .sharingMode = VK_SHARING_MODE_CONCURRENT, 
         .queueFamilyIndexCount = static_cast<std::uint32_t>(
            create_info.value( ).family_indices.size( )
         ),
         .pQueueFamilyIndices = create_info.value( ).family_indices.data( )
      };

      VmaAllocationCreateInfo index_alloc_info = { };
      index_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

      vmaCreateBuffer(
         memory_allocator,
         &index_create_info,
         &index_alloc_info,
         &buffer,
         &allocation,
         nullptr
      );

      /* CREATE COMMAND BUFFER */
      VkCommandBuffer cmd_buffer = VK_NULL_HANDLE;

      auto temp_cmd_buffer = create_info.value( ).p_context->create_command_buffers(
         queue::flag_t( queue::flag::e_transfer ),
         count32_t( 1 )
      );

      if ( auto const* p_val = 
            std::get_if<std::vector<VkCommandBuffer>>( &temp_cmd_buffer ) )
      {
         cmd_buffer = (*p_val)[0];
      }
      else
      {
         
         abort();
      }

      VkCommandBufferBeginInfo const begin_info
      {
         .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
         .pNext = nullptr,
         .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
         .pInheritanceInfo = nullptr
      };

      vkBeginCommandBuffer( cmd_buffer, &begin_info );

      VkBufferCopy const copy_region
      {
         .srcOffset = VkDeviceSize( 0 ),
         .dstOffset = VkDeviceSize( 0 ),
         .size = VkDeviceSize( buffer_size )
      };

      vkCmdCopyBuffer(
         cmd_buffer,
         staging_buffer, buffer,
         1,&copy_region
      );

      vkEndCommandBuffer( cmd_buffer );

      VkSubmitInfo const submit_info
      {
         .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
         .pNext = nullptr,
         .waitSemaphoreCount = 0,
         .pWaitSemaphores = nullptr,
         .pWaitDstStageMask = nullptr,
         .commandBufferCount = 1,
         .pCommandBuffers = &cmd_buffer,
         .signalSemaphoreCount = 0,
         .pSignalSemaphores = nullptr
      };

      create_info.value( ).p_context->submit_queue(
         queue::flag_t( queue::flag::e_transfer ),
         submit_info_t( submit_info ),
         fence_t( nullptr )
      );

      create_info.value( ).p_context->queue_wait_idle(
         queue::flag_t( queue::flag::e_transfer )
      );

      vmaDestroyBuffer( memory_allocator, staging_buffer, staging_memory );
   }

   index_buffer::index_buffer( index_buffer&& rhs )
   {
      *this = std::move( rhs );
   }

   index_buffer::~index_buffer( )
   {
      if ( memory_allocator != VK_NULL_HANDLE &&
           allocation != VK_NULL_HANDLE &&
           buffer != VK_NULL_HANDLE )
      {
         vmaDestroyBuffer( memory_allocator, buffer, allocation ); 

         buffer = VK_NULL_HANDLE;
         allocation = VK_NULL_HANDLE;
      }
   }

   index_buffer& index_buffer::operator=( index_buffer&& rhs )
   {
      if ( this != &rhs )
      {
         buffer = rhs.buffer;
         rhs.buffer = VK_NULL_HANDLE;

         allocation = rhs.allocation;
         rhs.allocation = VK_NULL_HANDLE;

         memory_allocator = rhs.memory_allocator;
         rhs.memory_allocator = VK_NULL_HANDLE;
      }
   }

   VkBuffer index_buffer::get_buffer( ) const
   {
      return buffer;
   }
} // namespace vk
