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

#include <cassert>

namespace vk::shader
{
   manager::manager( )
   {
      if ( !GLSLANG_INITIALIZED )
      {
         glslang::InitializeProcess( );
         GLSLANG_INITIALIZED = true;
      }
   }

   manager::manager( p_context_t const& p_context )
      :
      p_context( p_context.value( ) )
   {
      if ( !GLSLANG_INITIALIZED )
      {
         glslang::InitializeProcess( );
         GLSLANG_INITIALIZED = true;
      }
   }

   manager::manager( manager&& rhs )
   {
      *this = std::move( rhs );
   }

   manager::~manager( )
   {
      if ( p_save_interface )
      {
         for( auto const& shader : shaders )
         {
            
         }
      }
   }

   manager& manager::operator=( manager&& rhs )
   {
      if ( this != &rhs )
      {
         p_context = rhs.p_context;   
         rhs.p_context = nullptr;
         
         shaders = std::move( rhs.shaders );

         shader_packs = std::move( rhs.shader_packs );
      }

      return *this;
   }

   id manager::load_module( loader_ptr_t loader, filepath_t const& filepath )
   {
      if (loader.value() != nullptr)
      {
         // TODO: Handle error.
         abort();
      } 
    
      auto shader_data = loader.value( )->load_shader( vk::shader::filepath_view_t( filepath.value( ) ) );

      auto create_info = unique_shader::create_info( );
      create_info.p_context = p_context;
      create_info.spir_v = shader_data.first;
      create_info.shader_type = shader_data.second;
  
      std::uint32_t id = SHADER_ID_COUNT++;

      shaders.emplace( id, unique_shader( unique_shader::create_info_t( create_info ) ) );

      return id;
   }
   
   bool manager::delete_module( id_t id )
   {
      for ( auto& pack : shader_packs )
      {
         auto const vertex = pack.second.get_vertex_shader( );
         if ( vertex.has_value( ) && vertex.value( ) == id.value( ) )
         {
            return false;
         }

         auto const tess_eval = pack.second.get_tess_evaluation_shader( );
         if ( tess_eval.has_value( ) && tess_eval.value( ) == id.value( ) )
         {
            return false;
         }

         auto const tess_control = pack.second.get_tess_control_shader( );
         if ( tess_control.has_value( ) && tess_control.value( ) == id.value( ) )
         {
            return false;
         }

         auto const geometry = pack.second.get_geometry_shader( );
         if ( geometry.has_value( ) && geometry.value( ) == id.value( ) )
         {
            return false;
         }

         auto const fragment = pack.second.get_fragment_shader( );
         if ( fragment.has_value( ) && geometry.value( ) == id.value( ) )
         {
            return false;
         }

         auto const compute = pack.second.get_compute_shader( );
         if ( compute.has_value( ) && compute.value( ) == id.value( ) )
         {
            return false;
         }
      }
 
      if ( auto it = shaders.find( id.value( ) ); it != shaders.end( ) )
      {
         shaders.erase( it );
      }

      return true;
   }

   std::vector<VkPipelineShaderStageCreateInfo> manager::get_shader_stage_create_infos( set::id_t shader_pack_id ) const
   {
      auto const pack_it = shader_packs.find( shader_pack_id.value( ) );
      
      assert( ( "shader pack " + std::to_string( shader_pack_id.value( ) ) + " is not present", pack_it == shader_packs.end( ) ) );

      std::vector<VkPipelineShaderStageCreateInfo> create_infos;
      create_infos.reserve( 6 );

      if ( pack_it->second.get_vertex_shader( ).has_value( ) )
      {
         auto const shader_it = shaders.find( pack_it->second.get_vertex_shader( ).value( ) );
        
         auto create_info = VkPipelineShaderStageCreateInfo{ };
         create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
         create_info.pNext = nullptr;
         create_info.flags = 0;
         create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
      }
   }

   std::uint32_t manager::create_pack( set::create_info_t const& create_info )
   {
      std::uint32_t id = SHADER_PACK_ID_COUNT++;

      shader_packs.emplace( id, set( create_info ) );

      return id;
   }

   void manager::enable_shader_saving( save_uptr_t p_saving )
   {
      p_save_interface.reset( p_saving.value( ).release( ) );   
   }

   void manager::disable_shader_saving( )
   {
      if ( p_save_interface )
      {
         p_save_interface.reset( );
      }
   }
} // namespace vk
