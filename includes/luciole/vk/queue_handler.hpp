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

#ifndef LUCIOLE_VULKAN_QUEUE_HANDLER_HPP
#define LUCIOLE_VULKAN_QUEUE_HANDLER_HPP

#include <luciole/luciole_core.hpp>
#include <luciole/utils/logger.hpp>
#include <luciole/vk/queue.hpp>

#include <unordered_map>

namespace vk
{
   class queue_handler
   {
   public:
      queue_handler( );
      queue_handler( VkDevice device, logger* p_logger, std::vector<VkQueueFamilyProperties> const& queue_properties );

      void make_queue_wait_idle( queue::flag flags );

      void submit_queue( queue::flag flags, VkSubmitInfo const& submit_info, VkFence fence );
      vk::error present_queue( queue::flag flags, VkPresentInfoKHR const& present_info );

      VkSharingMode get_sharing_mode( ) const noexcept;

   private:
      logger* p_logger;

      std::unordered_map<queue::flag, queue> queues;
   };
} // namespace vk

#endif // LUCIOLE_VULKAN_QUEUE_HANDLER_HPP
