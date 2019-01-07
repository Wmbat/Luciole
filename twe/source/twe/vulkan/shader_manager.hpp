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
        
        TWE_API shader::id insert( const shader::create_info& create_info );
        
        TWE_API const shader& find( const shader::id id ) const;
        
        TWE_API const shader& operator[]( const shader::id id ) const;
    
    private:
        std::unordered_map<shader::id, shader> shaders_;
        
        static inline shader::id shader_id_count_;
    };
}

#endif //TWE_VK_SHADER_MANAGER_H
