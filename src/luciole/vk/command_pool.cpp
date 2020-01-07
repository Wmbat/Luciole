/*
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

#include <luciole/vk/command_pool.hpp>
#include <luciole/utils/logger.hpp>

namespace vk
{
   command_pool::command_pool( ) : p_logger( nullptr ), device( VK_NULL_HANDLE ), handle( VK_NULL_HANDLE ), pool_allocation_count( 0 ) {}

   command_pool::command_pool( VkDevice device, std::uint32_t queue_family_index, logger* p_logger ) :
      p_logger( p_logger ), device( device ), handle( VK_NULL_HANDLE ), pool_allocation_count( 0 )
   {
      VkCommandPoolCreateInfo create_info = {};
      create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      create_info.pNext = nullptr;
      create_info.flags = 0;
      create_info.queueFamilyIndex = queue_family_index;

      auto const result = vkCreateCommandPool( device, &create_info, nullptr, &handle );
      if ( result != VK_SUCCESS )
      {
         if ( p_logger )
         {
            p_logger->error( "[{0}] Failed to create command_pool because: {1}", __FUNCTION__, result );
         }
      }
   }

   command_pool::command_pool( command_pool&& other ) { *this = std::move( other ); }

   command_pool::~command_pool( )
   {
      if ( handle != VK_NULL_HANDLE && device != VK_NULL_HANDLE )
      {
         vkDestroyCommandPool( device, handle, nullptr );
         device = VK_NULL_HANDLE;
         handle = VK_NULL_HANDLE;
      }

      p_logger = nullptr;
   }

   command_pool& command_pool::operator=( command_pool&& rhs )
   {
      if ( this != &rhs )
      {
         p_logger = rhs.p_logger;
         rhs.p_logger = nullptr;

         device = rhs.device;
         rhs.device = VK_NULL_HANDLE;

         handle = rhs.handle;
         rhs.handle = VK_NULL_HANDLE;

         command_buffers = std::move( rhs.command_buffers );

         pool_allocation_count = rhs.pool_allocation_count;
         rhs.pool_allocation_count = 0;
      }

      return *this;
   }

   std::uint32_t command_pool::allocate_command_buffers( std::uint32_t count )
   {
      std::uint32_t id = pool_allocation_count++;

      VkCommandBufferAllocateInfo alloc_info = {};
      alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      alloc_info.pNext = nullptr;
      alloc_info.commandPool = handle;
      alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      alloc_info.commandBufferCount = count;

      std::vector<VkCommandBuffer> buffers( count );

      auto const result = vkAllocateCommandBuffers( device, &alloc_info, buffers.data( ) );
      if ( result != VK_SUCCESS )
      {
      }

      command_buffers.emplace( id, buffers );

      return id;
   }

   void command_pool::free_command_buffers( std::uint32_t allocation_index )
   {
      auto const it = command_buffers.find( allocation_index );
      if ( it != command_buffers.cend( ) )
      {
         vkFreeCommandBuffers( device, handle, it->second.size( ), it->second.data( ) );

         command_buffers.erase( it );
      }
   }
} // namespace vk
