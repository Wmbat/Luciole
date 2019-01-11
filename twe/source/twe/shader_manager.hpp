/*
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

#ifndef TWE_VK_SHADER_MANAGER_H
#define TWE_VK_SHADER_MANAGER_H

#include <unordered_map>

#include "shader.hpp"

namespace twe
{
    class shader_manager
    {
    public:
        TWE_API shader_manager( ) = default;
        TWE_API shader_manager( const shader_manager& rhs ) = delete;
        TWE_API shader_manager( shader_manager&& rhs ) noexcept;
        TWE_API ~shader_manager( ) = default;
        
        TWE_API shader_manager& operator=( const shader_manager& rhs ) = delete;
        TWE_API shader_manager& operator=( shader_manager&& rhs ) noexcept;
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, vertex_shader>, uint32_t> insert( const shader_create_info& create_info )
        {
            auto id = ++shader_id_count_;
            
            vertex_shaders_.emplace( std::pair{ id, vertex_shader{ create_info } } );
            
            return id;
        }
        template<class C>
        std::enable_if_t<std::is_same_v<C, fragment_shader>, uint32_t> insert( const shader_create_info& create_info )
        {
            auto id = ++shader_id_count_;
            
            fragment_shaders_.emplace( std::pair{ id, fragment_shader{ create_info } } );
            
            return id;
        }
        
        template<class C, class = std::enable_if_t<std::is_same_v<C, vertex_shader>>>
        const vertex_shader& find( const uint32_t id ) const
        {
            const auto it = vertex_shaders_.find( id );
            
            assert( it != vertex_shaders_.cend() );
            
            return it->second;
        }
        template<class C, class = std::enable_if_t<std::is_same_v<C, fragment_shader>>>
        const fragment_shader& find( const uint32_t id ) const
        {
            const auto it = fragment_shaders_.find( id );
            
            assert( it != fragment_shaders_.cend() );
            
            return it->second;
        }
    
    private:
        std::unordered_map<uint32_t, vertex_shader> vertex_shaders_;
        std::unordered_map<uint32_t, fragment_shader> fragment_shaders_;
        
        static inline uint32_t shader_id_count_;
    };
}

#endif //TWE_VK_SHADER_MANAGER_H
