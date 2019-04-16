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

#ifndef LUCIOLE_VULKAN_SHADER_MANAGER_H
#define LUCIOLE_VULKAN_SHADER_MANAGER_H

#include <unordered_map>

#include "shader.hpp"

namespace lcl::vulkan
{
    class shader_manager
    {
    public:
        LUCIOLE_API shader_manager( ) = default;
        LUCIOLE_API shader_manager( const shader_manager& rhs ) = delete;
        LUCIOLE_API shader_manager( shader_manager&& rhs ) noexcept;
        LUCIOLE_API ~shader_manager( ) = default;
        
        LUCIOLE_API shader_manager& operator=( const shader_manager& rhs ) = delete;
        LUCIOLE_API shader_manager& operator=( shader_manager&& rhs ) noexcept;
        
        template<shader_type type>
        std::uint32_t insert( const shader_create_info& create_info )
        {
            const auto id = ++shader_id_count_;

            if constexpr ( type == shader_type::e_vertex )
            {
                vertex_shaders_.emplace( std::pair{ id, vertex_shader{ create_info } } );
            }
            else if ( type == shader_type::e_fragment )
            {
                fragment_shaders_.emplace( std::pair{ id, fragment_shader{ create_info } } );
            }

            return id;
        }
        
        template<shader_type type>
        const shader<type>& find( std::uint32_t id ) const
        {
            if constexpr ( type == shader_type::e_vertex )
            {
                const auto it = vertex_shaders_.find( id );

                // TODO: throw exception.

                return it->second;
            }
            else
            {
                const auto it = fragment_shaders_.find( id );

                return it->second;
            }
        }
    
    private:
        std::unordered_map<uint32_t, vertex_shader> vertex_shaders_;
        std::unordered_map<uint32_t, fragment_shader> fragment_shaders_;
        
        static inline uint32_t shader_id_count_;
    };
}

#endif //LUCIOLE_VULKAN_SHADER_MANAGER_H
