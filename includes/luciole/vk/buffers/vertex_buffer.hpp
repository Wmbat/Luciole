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
#include <luciole/strong_types.hpp>
#include <luciole/graphics/vertex.hpp>
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
      /**
       * @brief Default constructor.
       */
      vertex_buffer( ) = default;
      /**
       * @brief Constructor.
       *
       * @param [in] create_info The information required
       * to create the vertex_buffer object.
       */
      vertex_buffer( create_info_t const& create_info );
      /**
       * @brief Deleted copy constructor.
       *
       * @param [in] rhs The vertex_buffer object to copy from.
       */
      vertex_buffer( vertex_buffer const& rhs ) = delete;
      /**
       * @brief Move constructor.
       *
       * @param [in/out] rhs The vertex_buffer object to move
       * from.
       */
      vertex_buffer( vertex_buffer&& rhs );
      /**
       * @brief Destructor.
       */
      ~vertex_buffer();

      /**
       * @brief Deleted copy assigment operator.
       *
       * @param [in] The vertex_buffer object to copy.
       *
       * @return The current vertex_buffer.
       */
      vertex_buffer& operator=( vertex_buffer const& rhs ) = delete;
      /**
       * @brief Move assigment operator.
       *
       * @param [in/out] The vertex_buffer object to move.
       *
       * @return The current vertex_buffer.
       */
      vertex_buffer& operator=( vertex_buffer&& rhs );

      [[nodiscard]]
      inline VkBuffer get_buffer(
      ) const PURE
      {
         return buffer_;
      }

   private: 
      VmaAllocator memory_allocator_ = VK_NULL_HANDLE;
      VmaAllocation memory_allocation_ = VK_NULL_HANDLE;
      VkBuffer buffer_ = VK_NULL_HANDLE;
   }; // class vertex_buffer
} // namespace vk

#endif // LUCIOLE_VK_BUFFERS_VERTEX_BUFFER_HPP
