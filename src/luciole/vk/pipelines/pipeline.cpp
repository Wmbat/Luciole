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

#include <luciole/graphics/vertex.hpp>
#include <luciole/vk/pipelines/pipeline.hpp>

namespace vk::pipeline
{
   unique_pipeline::unique_pipeline( )
      :
      p_context( nullptr ),
      pipeline_type( type::e_count ),
      handle( VK_NULL_HANDLE )
   {}

   unique_pipeline::unique_pipeline( create_info_t const& create_info )
      :
      p_context( create_info.value( ).p_context ),
      pipeline_type( create_info.value().pipeline_type ),
      handle( VK_NULL_HANDLE )
   {
      auto const shader_stages = create_info.value( ).p_shader_manager->get_shader_stage_create_infos( 
            shader::set::id_t( create_info.value( ).shader_set_id ) 
      );

      auto const binding_description = vertex::get_binding_description( );
      auto const attribute_description = vertex::get_attribute_descriptions( );
   }

   unique_pipeline::unique_pipeline( unique_pipeline&& rhs )
   {
      *this = std::move( rhs );
   }

   unique_pipeline::~unique_pipeline( )
   {
      if ( p_context != nullptr && handle != VK_NULL_HANDLE )
      {
         p_context->destroy_pipeline( vk::pipeline_t( handle ) );
         handle = VK_NULL_HANDLE;

         p_context = nullptr;
      }
   }

   unique_pipeline& unique_pipeline::operator=( unique_pipeline&& rhs )
   {
      if ( this != &rhs )
      {
         p_context = rhs.p_context;
         rhs.p_context = nullptr;

         pipeline_type = rhs.pipeline_type;
         rhs.pipeline_type = type::e_count;
            
         handle = rhs.handle;
         rhs.handle = VK_NULL_HANDLE;
      }

      return *this; 
   }
} // namespace vk
