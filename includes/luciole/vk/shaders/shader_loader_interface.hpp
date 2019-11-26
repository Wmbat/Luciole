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

#ifndef LUCIOLE_VK_SHADERS_SHADER_LOADER_INTERFACE_HPP
#define LUCIOLE_VK_SHADERS_SHADER_LOADER_INTERFACE_HPP

#include <luciole/utils/strong_types.hpp>
#include <luciole/vk/shaders/shader.hpp>

#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>
#include <glslang/Public/ShaderLang.h>

#include <vector>

namespace vk::shader
{
   class loader_interface
   {
   public:
      using shader_data = std::pair<std::vector<std::uint32_t>, type>;

   public:
      loader_interface( ) = default;
      virtual ~loader_interface( ) = default;

      /**
       * @brief Pure virtual function to override for a custom
       * implemenation to load shaders and return the SPIR-V binary.
       *
       * @return The SPIR-V binary for the shader and the shader type.
       */
      virtual shader_data load_shader( shader::filepath_view_t filepath ) const = 0;
   }; // class shader_loader_interface

   using loader_ptr_t = strong_type<loader_interface const*, loader_interface>;
} // namespace vk::shader

#endif // LUCIOLE_VK_SHADERS_SHADER_LOADER_INTERFACE_HPP
