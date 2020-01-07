/**
 *  Copyright (C) 2020 Wmbat
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

#ifndef LUCIOLE_CORE_CONTEXT_HPP
#define LUCIOLE_CORE_CONTEXT_HPP

#include <luciole/core/common_types.hpp>
#include <luciole/vk/core.hpp>
#include <luciole/vk/shaders/shader.hpp>

#include <vector>

class logger;

namespace ui
{
   class window;
} // namespace ui

namespace core
{
   class physical_device_selection_interface;

   class context
   {
   public:
      context( );
      context( ui::window const& window, logger* p_logger );
      context( context const& other ) = delete;
      context( context&& other );
      ~context( );

      context& operator=( context const& rhs ) = delete;
      context& operator=( context&& rhs );

      void select_best_physical_device( physical_device_selection_interface const* p_selection );

      VkShaderModule create_shader_module( vk::shader::unique_shader::create_info const& create_info ) const;

   private:
      void init_volk( logger* p_logger ) const;

   private:
      logger* p_logger;

      VkInstance instance;
      VkDebugUtilsMessengerEXT debug_messenger;
      VkPhysicalDevice physical_device;
      VkDevice logical_device;

      std::vector<VkPhysicalDevice> available_physical_devices;

      inline static bool VOLK_IS_INITIALIZED = false;
   }; // class context
} // namespace vk

#endif // LUCIOLE_CORE_CONTEXT_HPP
