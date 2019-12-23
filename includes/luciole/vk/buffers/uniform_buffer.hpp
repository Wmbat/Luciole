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

#ifndef LUCIOLE_VK_BUFFERS_UNIFORM_BUFFER_HPP
#define LUCIOLE_VK_BUFFERS_UNIFORM_BUFFER_HPP

#include <luciole/context.hpp>
#include <luciole/ui/window.hpp>
#include <luciole/utils/strong_types.hpp>
#include <luciole/vk/core.hpp>
#include <luciole/vk/vma/vk_mem_alloc.h>

namespace vk
{
   class uniform_buffer
   {
   public:
      uniform_buffer( ) = default;
      uniform_buffer( context const& ctx, std::size_t buffer_size );
      uniform_buffer( uniform_buffer const& rhs ) = delete;
      uniform_buffer( uniform_buffer&& rhs );
      ~uniform_buffer( );

      uniform_buffer& operator=( uniform_buffer const& rhs ) = delete;
      uniform_buffer& operator=( uniform_buffer&& rhs );

      template <typename T>
      void map_data( T const& data )
      {
         void* local_data;

         vmaMapMemory( memory_allocator, allocation, &local_data );
         memcpy( local_data, &data, sizeof( data ) );
         vmaUnmapMemory( memory_allocator, allocation );
      }

   private:
      VmaAllocator memory_allocator = VK_NULL_HANDLE;
      VmaAllocation allocation = VK_NULL_HANDLE;
      VkBuffer buffer = VK_NULL_HANDLE;
   }; // class uniform_buffer
} // namespace vk

#endif
