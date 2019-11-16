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

#include <luciole/utils/file_io.hpp>
#include <luciole/vk/shaders/shader_manager.hpp>

namespace vk
{

   shader_manager::shader_manager( )
   {
      if ( !GLSLANG_INITIALIZED )
      {
         glslang::InitializeProcess( );
         GLSLANG_INITIALIZED = true;
      }
   }

   shader_manager::shader_manager( p_context_t const& p_context )
      :
      p_context( p_context.value( ) )
   {
      if ( !GLSLANG_INITIALIZED )
      {
         glslang::InitializeProcess( );
         GLSLANG_INITIALIZED = true;
      }
   }

   shader_manager::shader_manager( shader_manager&& rhs )
   {
      *this = std::move( rhs );
   }

   shader_manager::~shader_manager( )
   {

   }

   shader_manager& shader_manager::operator=( shader_manager&& rhs )
   {
      if ( this != &rhs )
      {
         
      }

      return *this;
   }

   std::uint32_t shader_manager::load_shader( shader_loader_interface const* loader, shader::filepath_t const& filepath )
   {
      auto shader_data = loader->load_shader( vk::shader::filepath_view_t( filepath.value( ) ) );
     
      auto create_info = shader::create_info( );
      create_info.p_context = p_context;
      create_info.spir_v = shader_data.first;
      create_info.shader_type = shader_data.second;
  
      std::uint32_t id = SHADER_ID_COUNT++;

      shaders.emplace( id, shader( shader::create_info_t( create_info ) ) );

      return id;
   }
} // namespace vk
