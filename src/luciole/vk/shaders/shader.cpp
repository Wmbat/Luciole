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

#include <luciole/vk/shaders/shader.hpp>

namespace vk
{
   shader::shader( )
      :
      p_context( nullptr ),
      shader_type( type::e_count ),
      handle( VK_NULL_HANDLE )
   {  }

   shader::shader( create_info_t const& create_info )
      :
      p_context( create_info.value( ).p_context ),
      shader_type( type::e_count ),
      handle( VK_NULL_HANDLE )
   {  
      auto module_create_info = VkShaderModuleCreateInfo{ };
      module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
      module_create_info.pNext = nullptr;
      module_create_info.flags = { };
      module_create_info.pCode = create_info.value( ).spir_v.data( );
      module_create_info.codeSize = static_cast<std::uint32_t>( create_info.value( ).spir_v.size( ) );

      handle = p_context->create_shader_module( shader_module_create_info_t( module_create_info ) );

      // TODO: handle error
   }

   shader::~shader( )
   {
      if ( p_context != nullptr && handle != nullptr )
      {
         p_context->destroy_shader_module( vk::shader_module_t( handle ) );
         handle = VK_NULL_HANDLE;

         p_context = nullptr;
      }
   }

   shader::shader( shader&& other )
   {
      *this = std::move( other );
   }

   shader& shader::operator=( shader&& rhs )
   {
      if ( this != &rhs )
      {
         p_context = rhs.p_context;
         rhs.p_context = nullptr;
         
         shader_type = rhs.shader_type;
         rhs.shader_type = type::e_count;
            
         handle = rhs.handle;
         rhs.handle = nullptr;
      }

      return *this;
   }
} // namespace vk
