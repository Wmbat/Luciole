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

#ifndef LUCIOLE_VK_COMMAND_POOL_HPP
#define LUCIOLE_VK_COMMAND_POOL_HPP

#include <luciole/vk/core.hpp>

#include <unordered_map>
#include <vector>

class context;
class logger;

namespace vk
{
   class command_pool
   {
   public:
      command_pool( );
      command_pool( VkDevice device, std::uint32_t queue_family_index, logger* p_logger );
      command_pool( command_pool const& rhs ) = delete;
      command_pool( command_pool&& rhs );
      ~command_pool( );

      command_pool& operator=( command_pool const& rhs ) = delete;
      command_pool& operator=( command_pool&& rhs );

      std::uint32_t allocate_command_buffers( std::uint32_t count );
      void free_command_buffers( std::uint32_t allocation_index );

   private:
      logger* p_logger;

      VkDevice device;
      VkCommandPool handle;

      std::unordered_map<std::uint32_t, std::vector<VkCommandBuffer>> command_buffers;

      std::uint32_t pool_allocation_count;
   }; // class command_pool
} // namespace vk

#endif // LUCIOLE_VK_COMMAND_POOL_HPP
