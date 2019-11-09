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

namespace vk
{
   class shader
   {
   protected:
      shader( ); 
      shader( p_context_t const& p_context );
      shader( shader const& rhs ) = delete;
      shader( shader&& rhs );
      ~shader( );       
        
      shader& operator=( shader const& rhs ) = delete;
      shader& operator=( shader&& rhs ); 

   protected:
      context const* p_context;

      VkShaderModule handle;
   }; // class 
} // namespace 

#endif // LUCIOLE_VK_SHADER_HPP

