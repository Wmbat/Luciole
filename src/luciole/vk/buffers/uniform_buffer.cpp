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

#include <luciole/vk/buffers/uniform_buffer.hpp>

namespace vk
{
   uniform_buffer::uniform_buffer( context const& ctx, std::size_t buffer_size )
      :
      memory_allocator_( ctx.get_memory_allocator( ) )
   {
      auto const indices = ctx.get_unique_family_indices( );

      VkBufferCreateInfo const create_info
      {
         .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
         .pNext = nullptr,
         .flags = 0,
         .size = buffer_size,
         .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
         .sharingMode = VK_SHARING_MODE_CONCURRENT,
         .queueFamilyIndexCount = static_cast<std::uint32_t>(
            indices.size()
         ),
         .pQueueFamilyIndices = indices.data()
      };

      VmaAllocationCreateInfo alloc_info = { };
      alloc_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;

      vmaCreateBuffer(
         memory_allocator_,
         &create_info,
         &alloc_info,
         &buffer_,
         &allocation_,
         nullptr
      );
   }   

   uniform_buffer::uniform_buffer( uniform_buffer&& rhs )
   {
      *this = std::move( rhs );
   }
   
   uniform_buffer::~uniform_buffer( )
   {
      if ( buffer_ != VK_NULL_HANDLE || 
           memory_allocator_ != VK_NULL_HANDLE || 
           allocation_ != VK_NULL_HANDLE )
      {
         vmaDestroyBuffer( memory_allocator_, buffer_, allocation_ );  
      }
   }
   
   uniform_buffer& uniform_buffer::operator=( uniform_buffer&& rhs )
   {
      if ( this != &rhs )
      {
         memory_allocator_ = rhs.memory_allocator_;
         rhs.memory_allocator_ = VK_NULL_HANDLE;

         allocation_ = rhs.allocation_;
         rhs.allocation_ = VK_NULL_HANDLE;

         buffer_ = rhs.buffer_;
         rhs.buffer_ = VK_NULL_HANDLE;
      }
   }

} // namespace vk
