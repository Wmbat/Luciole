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

#ifndef LUCIOLE_VK_BUFFERS_VERTEX_BUFFER_HPP
#define LUCIOLE_VK_BUFFERS_VERTEX_BUFFER_HPP

#include <luciole/context.hpp>
#include <luciole/graphics/vertex.hpp>
#include <luciole/utils/strong_types.hpp>
#include <luciole/vk/queue.hpp>

#include <vulkan/vulkan.h>

namespace vk
{
   /**
    * @brief A class that contains all the information
    * for vulkan vertex buffers.
    */
   class vertex_buffer
   {
   public:
      struct create_info
      {
         context const* p_context = nullptr;

         VmaAllocator memory_allocator = VK_NULL_HANDLE;

         std::vector<std::uint32_t> family_indices = {};
         std::vector<vertex> vertices = {};
      }; // struct create_info
      
      using create_info_t = strong_type<create_info const&>;

   public:
      vertex_buffer( ) = default;
      vertex_buffer( create_info_t const& create_info );
      vertex_buffer( vertex_buffer const& rhs ) = delete;
      vertex_buffer( vertex_buffer&& rhs );
      ~vertex_buffer();

      vertex_buffer& operator=( vertex_buffer const& rhs ) = delete;
      vertex_buffer& operator=( vertex_buffer&& rhs );

      [[nodiscard]]
      inline VkBuffer get_buffer(
      ) const PURE
      {
         return buffer;
      }

   private: 
      VmaAllocator memory_allocator = VK_NULL_HANDLE;
      VmaAllocation allocation = VK_NULL_HANDLE;
      VkBuffer buffer = VK_NULL_HANDLE;
   }; // class vertex_buffer
} // namespace vk

#endif // LUCIOLE_VK_BUFFERS_VERTEX_BUFFER_HPP
