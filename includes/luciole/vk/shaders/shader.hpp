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

#ifndef LUCIOLE_VK_SHADER_HPP
#define LUCIOLE_VK_SHADER_HPP

#include <luciole/context.hpp>
#include <luciole/utils/strong_types.hpp>
#include <luciole/vk/core.hpp>

#include <string>
#include <string_view>

namespace vk
{    
   class shader
   {
   public: 
      using filepath_t = strong_type<std::string const&, shader>; 
      using filepath_view_t = strong_type<std::string_view, shader>;
      
      enum class type
      {
         e_vertex,
         e_fragment,
         e_tess_control,
         e_tess_eval,
         e_geometry,
         e_compute,
         e_count
      }; // enum class type

      struct create_info
      {
         context const* p_context = nullptr;

         type shader_type = type::e_count;

         std::vector<std::uint32_t> spir_v;
      }; // struct create_info

      using create_info_t = strong_type<create_info const&, shader>;

   public:
      shader( ); 
      shader( create_info_t const& create_info );
      shader( shader const& rhs ) = delete;
      shader( shader&& rhs );
      ~shader( );       
        
      shader& operator=( shader const& rhs ) = delete;
      shader& operator=( shader&& rhs ); 

   private:
      context const* p_context;

      type shader_type;

      VkShaderModule handle;
   }; // class 
} // namespace 

#endif // LUCIOLE_VK_SHADER_HPP
