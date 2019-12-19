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

#include <luciole/context.hpp>
#include <luciole/vk/core.hpp>

namespace vk
{
   class index_buffer
   {
   public:
      struct create_info
      {
         context* p_context = nullptr;

         std::vector<std::uint32_t> family_indices = {};
         std::vector<std::uint32_t> indices = {};
      }; // struct create_info

   public:
      index_buffer( ) = default;
      index_buffer( create_info const& create_info );
      index_buffer( index_buffer const& rhs ) = delete;
      index_buffer( index_buffer&& rhs );
      ~index_buffer( );

      index_buffer& operator=( index_buffer const& rhs ) = delete;
      index_buffer& operator=( index_buffer&& rhs );

      /**
       * @brief Getter for the VkBuffer
       *
       * @return The underlying VkBuffer handle.
       */
      [[nodiscard]] VkBuffer get_buffer( ) const PURE;

   private:
      VmaAllocator memory_allocator = VK_NULL_HANDLE;
      VmaAllocation allocation = VK_NULL_HANDLE;
      VkBuffer buffer = VK_NULL_HANDLE;
   }; // class index_buffer
} // namespace vk
