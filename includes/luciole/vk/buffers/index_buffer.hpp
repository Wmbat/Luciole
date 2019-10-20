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
         context const* p_context = nullptr;

         std::vector<std::uint32_t> family_indices = {};
         std::vector<std::uint32_t> indices = {};
      }; // struct create_info

      using create_info_t = strong_type<create_info const&>;

   public:
      /**
       * @brief default constructor.
       */
      index_buffer( ) = default;
      /**
       * @brief Constructor.
       *
       * @param [in] create_info The information required to
       * create the buffer.
       */
      index_buffer( create_info_t const& create_info );
      /**
       * @brief Deleted Copy Constructor.
       *
       * @param [in] rhs The index_buffer to copy from.
       */
      index_buffer( index_buffer const& rhs ) = delete;
      /**
       * @brief Move Constructor.
       *
       * @param [in/out] rhs The index_buffer to move the
       * data from.
       */
      index_buffer( index_buffer&& rhs );
      /**
       * @brief Destructor.
       */
      ~index_buffer( );

      /**
       * @brief Deleted copy assigment operator.
       *
       * @param [in] rhs The data to copy from.
       *
       * @return The current index_buffer.
       */
      index_buffer& operator=( index_buffer const& rhs ) = delete;

      /**
       * @brief Copy assigment operator.
       *
       * @param [in/out] rhs The data to move.
       *
       * @return The current index_buffer.
       */
      index_buffer& operator=( index_buffer&& rhs );

      /**
       * @brief Getter for the VkBuffer
       *
       * @return The underlying VkBuffer handle.
       */
      [[nodiscard]]
      VkBuffer get_buffer(
      ) const PURE;

   private:
      VmaAllocator memory_allocator_ = VK_NULL_HANDLE;
      VmaAllocation allocation_ = VK_NULL_HANDLE;
      VkBuffer buffer_ = VK_NULL_HANDLE;
   }; // class index_buffer
} // namespace vk
