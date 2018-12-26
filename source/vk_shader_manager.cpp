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

#include <vk_shader_manager.h>

#include "vk_shader_manager.h"
#include "utilities/basic_error.h"

namespace TWE
{
    vk_shader::id vk_shader_manager::shader_id_count_;
    
    vk_shader::id vk_shader_manager::insert( const vk_shader::create_info& create_info )
    {
        auto id = ++shader_id_count_;
        
        shaders_.emplace( std::pair( id, std::make_shared<vk_shader>( create_info ) ) );
        
        return id;
    }
    
    const std::shared_ptr<vk_shader> vk_shader_manager::acquire( const vk_shader::id id ) const
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
    
    void vk_shader_manager::remove_orphans( )
    {
        for( auto it = shaders_.begin(); it != shaders_.cend(); )
        {
            if( it->second.unique() )
            {
                it = shaders_.erase( it );
            }
            else
            {
                ++it;
            }
        }
    }
}