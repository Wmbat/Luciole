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

#include <luciole/vk/queue_handler.hpp>

namespace vk
{
   queue_handler::queue_handler( ) : p_logger( nullptr ) {}
   queue_handler::queue_handler( VkDevice device, logger* p_logger, std::vector<VkQueueFamilyProperties> const& queue_properties ) : p_logger( p_logger )
   {
      auto graphics_queue = find_graphics_queue( device, queue_properties );
   }

   /*
    * @brief Make a queue wait idle.
    */
   void queue_handler::make_queue_wait_idle( queue::flag flags )
   {
      if ( auto queue = queues.find( flags ); queue != queues.cend( ) )
      {
         auto result = queue->second.wait_idle( );
         if ( result.is_error( ) )
         {
            if ( p_logger )
            {
               p_logger->error( "[{0}] Failed to make queue wait idle {1}", __FUNCTION__, result.to_string( ) );
            }
         }
      }
      else
      {
         if ( p_logger )
         {
            p_logger->warn( "[{0}] Could not find a queue with the flag {1}", __FUNCTION__, flags );
         }
      }
   }

   void queue_handler::submit_queue( queue::flag flags, const VkSubmitInfo& submit_info, VkFence fence )
   {
      if ( auto queue = queues.find( flags ); queue != queues.cend( ) )
      {
         auto result = queue->second.submit( vk::submit_info_t( submit_info ), vk::fence_t( fence ) );
         if ( result.is_error( ) )
         {
            if ( p_logger )
            {
               p_logger->error( "[{0}] Failed to submit queue {1}", __FUNCTION__, result.to_string( ) );
            }
         }
      }
      else
      {
         if ( p_logger )
         {
            p_logger->warn( "[{0}] Could not find a queue with the flag {1}", __FUNCTION__, flags );
         }
      }
   }

   vk::error queue_handler::present_queue( queue::flag flags, const VkPresentInfoKHR& present_info )
   {
      if ( auto queue = queues.find( flags ); queue != queues.cend( ) )
      {
         return queue->second.present( vk::present_info_t( present_info ) );
      }
      else
      {
         if ( p_logger )
         {
            p_logger->warn( "[{0}] Could not find a queue with the flag {1}", __FUNCTION__, flags );
         }
      }
   }

   /*
    * @brief Check if the handler is capable of supporting more than
    * one queue.
    */
   VkSharingMode queue_handler::get_sharing_mode( ) const noexcept
   {
      return queues.size( ) == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
   }

   std::optional<queue> queue_handler::find_graphics_queue( VkDevice device, std::vector<VkQueueFamilyProperties> const& queue_properties )
   {
      for ( std::size_t i = 0; i < queue_properties.size( ); ++i )
      {
         if ( queue_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT )
         {
            if ( p_logger )
            {
               p_logger->info( "[{0}] Graphics queue selected from queue family {1} at index {2}", __FUNCTION__, i, 0 );
            }

            return queue( device, i, 0 );
         }
      }

      return std::nullopt;
   }

   std::optional<queue> queue_handler::find_compute_queue( VkDevice device, std::vector<VkQueueFamilyProperties> const& queue_properties )
   {}

   std::optional<queue> queue_handler::find_transfer_queue( VkDevice device, std::vector<VkQueueFamilyProperties> const& queue_properties )
   {}
} // namespace vk
