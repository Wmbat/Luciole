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

#include "shader.h"

namespace TWE
{
    class shader_manager
    {
    public:
        shader::id insert( const shader::create_info& create_info );
        const shader& acquire( const shader::id id ) const;
    
    private:
        std::unordered_map<shader::id, shader> shaders_;
        
        static shader::id shader_id_count_;
    };
}

#endif //TWE_VK_SHADER_MANAGER_H
