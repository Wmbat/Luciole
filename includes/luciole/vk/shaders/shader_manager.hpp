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
#include <luciole/vk/shaders/shader_save_interface.hpp>
#include <luciole/vk/shaders/shader_set.hpp>
#include <luciole/context.hpp>

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>

#include <unordered_map>
#include <string_view>
#include <cstdint>
#include <vector>

namespace vk::shader
{ 
   class manager
   {
   public:
      manager( );
      manager( p_context_t const& p_context );
      manager( manager const& rhs ) = delete;
      manager( manager&& rhs );
      ~manager( );
   
      manager& operator=( manager const& rhs ) = delete;
      manager& operator=( manager&& rhs );
     
      id load_module( loader_ptr_t p_loader, filepath_t const& filepath );
      bool delete_module( id_t shader_id );  
      
      std::vector<VkPipelineShaderStageCreateInfo> get_shader_stage_create_infos( set::id_t shader_pack_id ) const;

      set::id create_pack( set::create_info_t const& create_info );
      bool delete_pack( set::id_t shader_pack_id );

      void enable_shader_saving( save_uptr_t p_saving );
      void disable_shader_saving( );
      
   private:
      context const* p_context; 

      std::unique_ptr<save_interface> p_save_interface;

      std::unordered_map<id, unique_shader> shaders; 
      std::unordered_map<set::id, set> shader_packs; 

      static inline bool GLSLANG_INITIALIZED = false;

      static inline std::uint32_t SHADER_ID_COUNT = 0;
      static inline std::uint32_t SHADER_PACK_ID_COUNT = 0;
   }; // class shader_manager
} // namespace vk

#endif // LUCIOLE_VK_SHADERS_SHADER_MANAGER_HPP
