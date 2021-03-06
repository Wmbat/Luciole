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

#include <luciole/vk/queue.hpp>

queue::queue( vk::device_t device, family_index_t family_index, index_t index )
   :
   family_index( family_index.value( ) ),
   index( index.value( ) )
{
   vkGetDeviceQueue( device.value( ), family_index.value( ), index.value( ), &handle );
}

/**
 * @brief Move constructor.
 */
queue::queue( queue&& rhs )
{
   *this = std::move( rhs );
}

/**
 * @brief Move assignment operator.
 */
queue& queue::operator=( queue&& rhs )
{
   if ( this != &rhs )
   {
      std::swap( handle, rhs.handle );
      std::swap( family_index, rhs.family_index );
      std::swap( index, rhs.index );
   }

   return *this;
}

/**
 * @brief Wait for the queue to finish a task.
 *
 * @return The result of the operation.
 */
vk::error queue::wait_idle( ) const noexcept
{
   vk::error const err( vk::result_t(
      vkQueueWaitIdle( handle )
   ) );

   return err;
}

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
vk::error queue::submit( 
   vk::submit_info_t const& info, 
   vk::fence_t const& fence ) const noexcept
{
   vk::error const err( vk::result_t(
      vkQueueSubmit( handle, 1, &info.value( ), fence.value( ) )
   ) );

   return err;
}

/**
 * @brief Present the queue.
 *
 * @param [in] info The information
 * necessary to present the queue.
 *
 * @return The result of the operation.
 */
vk::error queue::present( 
   vk::present_info_t const& info ) const noexcept
{
   vk::error const err( vk::result_t(
      vkQueuePresentKHR( handle, &info.value( ) )
   ) );

   return err;
}


/**
 * @brief Get the queue's family index.
 *
 * @return The queue's family index.
 */
std::uint32_t queue::get_family_index( ) const noexcept
{
   return family_index;
}
