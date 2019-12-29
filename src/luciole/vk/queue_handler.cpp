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

#include <algorithm>

namespace vk
{
   queue_handler::queue_handler( ) : p_logger( nullptr ) {}
   queue_handler::queue_handler( VkDevice device, logger* p_logger, std::vector<VkQueueFamilyProperties> const& queue_properties ) : p_logger( p_logger )
   {
      bool has_dedicated_transfer = false;
      for ( int i = 0; i < queue_properties.size( ); ++i )
      {
         if ( queue_properties[i].queueFlags == VK_QUEUE_TRANSFER_BIT )
         {
            queues.emplace( queue::flag::e_transfer, queue( device, i, 0 ) );
            has_dedicated_transfer = true;
         }
      }

      if ( !has_dedicated_transfer )
      {
         for ( int i = 0; i < queue_properties.size( ); ++i )
         {
            if ( queue_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT )
            {
               if ( !( queue_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT ) ||
                  !( queue_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ) )
               {
                  queues.emplace( queue::flag::e_transfer, queue( device, i, 0 ) );
                  has_dedicated_transfer = true;
               }
            }
         }
      }

      for ( int i = 0; i < queue_properties.size( ); ++i )
      {
         if ( queue_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT )
         {
            std::size_t index = 0;

            queues.emplace( queue::flag::e_graphics, queue( device, i, index++ ) );

            if ( !has_dedicated_transfer )
            {
               if ( queue_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT && queue_properties[i].queueCount > 1 )
               {
                  queues.emplace( queue::flag::e_transfer, queue( device, i, index++ ) );
               }
            }
         }
      }
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

   std::optional<std::uint32_t> queue_handler::get_queue_family_index( queue::flag flags ) const
   {
      auto const& it = queues.find( flags );
      if ( it == queues.cend( ) )
      {
         if ( p_logger )
         {
            p_logger->warn( "[{0}] Could not find a queue with the flags {1}", __FUNCTION__, flags );
         }

         return std::nullopt;
      }
      else
      {
         return it->second.get_family_index( );
      }
   }

   std::vector<std::uint32_t> queue_handler::get_queue_family_indices( ) const
   {
      std::vector<std::uint32_t> indices;
      indices.reserve( queues.size( ) );

      for ( auto const& queue : queues )
      {
         bool insert = true;
         for ( auto index : indices )
         {
            if ( queue.second.get_family_index( ) == index )
            {
               insert = false;
            }
         }

         if ( insert )
         {
            indices.push_back( queue.second.get_family_index( ) );
         }
      }

      return indices;
   }

   std::variant<std::unordered_map<std::uint32_t, VkCommandPool>, vk::error> queue_handler::generate_command_pool_infos(
      VkDevice device ) const
   {
      std::unordered_map<std::uint32_t, VkCommandPool> command_pools;
      command_pools.reserve( queues.size( ) );

      for ( auto const& queue : queues )
      {
         auto const& it = command_pools.find( queue.second.get_family_index( ) );
         if ( it != command_pools.cend( ) )
         {
            VkCommandPoolCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            create_info.pNext = nullptr;
            create_info.flags = 0;
            create_info.queueFamilyIndex = queue.second.get_family_index( );

            VkCommandPool handle = VK_NULL_HANDLE;
            auto result = vkCreateCommandPool( device, &create_info, nullptr, &handle );

            if ( result != VK_SUCCESS )
            {
               return vk::error( vk::result_t( result ) );
            }
            else
            {
               p_logger->info( "[{0}] Command pool 0x{1:x} created from queue family index {2}", __FUNCTION__,
                  reinterpret_cast<std::uintptr_t>( handle ), queue.second.get_family_index( ) );

               command_pools.emplace( queue.second.get_family_index( ), handle );
            }
         }
      }

      return command_pools;
   }
   /*
    * @brief Check if the handler is capable of supporting more than
    * one queue.
    */
   VkSharingMode queue_handler::get_sharing_mode( ) const noexcept
   {
      return queues.size( ) == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
   }
} // namespace vk
