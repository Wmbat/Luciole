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

#include "shader_manager.hpp"
#include "../utilities/basic_error.h"

namespace twe
{
    shader_manager::shader_manager( shader_manager&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    shader_manager& shader_manager::operator=( shader_manager&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            shaders_ = std::move( rhs.shaders_ );
            
            shader_id_count_ = rhs.shader_id_count_;
            rhs.shader_id_count_ = 0;
        }
        
        return *this;
    }
    
    shader::id shader_manager::insert( const shader::create_info& create_info )
    {
        auto id = ++shader_id_count_;
        
        shaders_.emplace( std::pair( id, shader{ create_info } ) );
        
        return id;
    }
    
    const shader& shader_manager::find( const shader::id id ) const
    {
        const auto i = shaders_.find( id );
        
        assert( i != shaders_.cend( ) );
        
        return i->second;
    }
    
    const shader& shader_manager::operator[]( const shader::id id ) const
    {
        const auto i = shaders_.find( id );
    
        assert( i != shaders_.cend( ) );
    
        return i->second;
    }
}