/*!
 *  Copyright (C) 2018 Wmbat
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

#include "graphics/shader_manager.h"
#include "utilities/basic_error.h"

namespace TWE
{
    shader::id shader_manager::shader_id_count_;
    
    shader::id shader_manager::insert( const shader::create_info& create_info )
    {
        auto id = ++shader_id_count_;
        
        shaders_.emplace( std::pair( id, shader{ create_info } ) );
        
        return id;
    }
    
    const shader& shader_manager::acquire( const shader::id id ) const
    {
        const auto i = shaders_.find( id );
        if( i != shaders_.cend() )
        {
            return i->second;
        }
        else
        {
            throw basic_error{ basic_error::error_code::vk_shader_not_present,
                               "Shader: " + std::to_string( id ) + "is not in the manager. Call insert first." };
        }
    }
}