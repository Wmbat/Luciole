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

#include <luciole/context.hpp>
#include <luciole/utils/logger.hpp>
#include <luciole/vk/core.hpp>
#include <luciole/vk/queue.hpp>

#include <vector>

namespace vk
{
   class command_pool
   {
   public:
      command_pool( );
      command_pool( context const* p_context, logger* p_logger, std::uint32_t queue_family_index );
      command_pool( command_pool const& rhs ) = delete;
      command_pool( command_pool&& rhs );
      ~command_pool( );

      command_pool& operator=( command_pool const& rhs ) = delete;
      command_pool& operator=( command_pool&& rhs );

   private:
      context const* p_context;
      logger* p_logger;

      VkCommandPool handle;
      std::vector<VkCommandBuffer> command_buffers;
   }; // class command_pool
} // namespace vk

