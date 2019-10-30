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

#ifndef LUCIOLE_VULKAN_QUEUE_HPP
#define LUCIOLE_VULKAN_QUEUE_HPP

#include <luciole/luciole_core.hpp>
#include <luciole/utilities/enum_operators.hpp>
#include <luciole/vk/core.hpp>
#include <luciole/vk/errors.hpp>

class queue
{
private:
   struct param { };
   struct index_param { };
   struct family_index_param { };

public:
   /**
    * @brief The possible operations
    * a queue is capable
    */
   enum class flag
   {
       e_none      = 0,
       e_graphics  = 1 << 1,
       e_transfer  = 1 << 2,
       e_compute   = 1 << 3
   };

   using flag_t = strong_type<flag, param>;
   using index_t = strong_type<std::uint32_t, index_param>;
   using family_index_t = strong_type<std::uint32_t, family_index_param>;

public:
   /**
    * @brief Default constructor.
    */
   queue( ) = default;

   queue( vk::device_t device, family_index_t family_index, index_t index );

   /**
    * @brief Deleted copy constructor.
    */
   queue( queue const& rhs ) = delete;

   /**
    * @brief Move constructor.
    */
   queue( queue && rhs );

   /**
    * @brief Deleted copy assigment operator.
    */
   queue& operator=( queue const& rhs ) = delete;

   /**
    * @brief Move assignment operator.
    */
   queue& operator=( queue && rhs );

   /**
    * @brief Wait for the queue to finish a task.
    *
    * @return The result of the operation.
    */
   [[nodiscard]] 
   vk::error wait_idle( 
   ) const noexcept PURE;
   
   /**
    * @brief Submit the queue.
    *
    * @param [in] info The information
    * necessary to submit the queue.
    * @param [in] fence The fence to handle
    * synchronization.
    *
    * @return The result of the operation.
    */
   [[nodiscard]] 
   vk::error submit( 
      vk::submit_info_t const& info, 
      vk::fence_t const& fence 
   ) const noexcept PURE;
   
   /**
    * @brief Present the queue.
    *
    * @param [in] info The information
    * necessary to present the queue.
    *
    * @return The result of the operation.
    */
   [[nodiscard]]
   vk::error present( 
      vk::present_info_t const& info 
   ) const noexcept PURE;
   
   /**
    * @brief Get the queue's family index.
    *
    * @return The queue's family index.
    */
   [[nodiscard]] 
   std::uint32_t get_family_index( 
   ) const noexcept PURE;

private:
   VkQueue handle_  = VK_NULL_HANDLE;
   std::uint32_t family_index_ = 0;
   std::uint32_t index_ = 0;
};

ENABLE_BITMASK_OPERATORS( queue::flag );

#endif // LUCIOLE_VULKAN_QUEUE_HPP
