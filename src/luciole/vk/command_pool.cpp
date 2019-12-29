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

#include <luciole/vk/command_pool.hpp>

namespace vk
{
   command_pool::command_pool( ) : p_context( nullptr ), p_logger( nullptr ), handle( VK_NULL_HANDLE ) {}

   command_pool::command_pool( context const* p_context, logger* p_logger, std::uint32_t queue_family_index ) :
      p_context( p_context ), p_logger( p_logger ), handle( VK_NULL_HANDLE )
   {
      VkCommandPoolCreateInfo create_info = {};
      create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      create_info.pNext = nullptr;
      create_info.flags = 0;
      create_info.queueFamilyIndex = queue_family_index;

      auto const result = vkCreateCommandPool( p_context->get( ), &create_info, nullptr, &handle );
      if ( result != VK_SUCCESS )
      {
         if ( p_logger )
         {
            p_logger->error( "[{0}] Failed to create command_pool because: {1}", __FUNCTION__, result );
         }
      }
   }
} // namespace vk
