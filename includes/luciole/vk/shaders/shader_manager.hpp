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

#ifndef LUCIOLE_VK_SHADERS_SHADER_MANAGER_HPP
#define LUCIOLE_VK_SHADERS_SHADER_MANAGER_HPP

#include <luciole/vk/shaders/shader.hpp>
#include <luciole/vk/shaders/shader_loader_interface.hpp>
#include <luciole/context.hpp>

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>

#include <unordered_map>
#include <string_view>
#include <cstdint>

namespace vk
{ 
   class shader_manager
   {
   public:
      shader_manager( );
      shader_manager( p_context_t const& p_context );
      shader_manager( shader_manager const& rhs ) = delete;
      shader_manager( shader_manager&& rhs );
      ~shader_manager( );
   
      shader_manager& operator=( shader_manager const& rhs ) = delete;
      shader_manager& operator=( shader_manager&& rhs );
  
      std::uint32_t load_shader( shader_loader_interface const* loader, shader::filepath_t const& filepath );
   
   private:
      context const* p_context; 

      std::unordered_map<std::uint32_t, shader> shaders;

      static inline bool GLSLANG_INITIALIZED = false;
      static inline std::uint32_t SHADER_ID_COUNT = 0;
   }; // class shader_manager
} // namespace vk

#endif // LUCIOLE_VK_SHADERS_SHADER_MANAGER_HPP
