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

#include "vk_shader.h"

namespace TWE
{
    class vk_shader_manager
    {
    public:
        std::uint32_t insert( const vk_shader::create_info& create_info );
        const vk_shader& acquire( const uint32_t id ) const;
        
        void remove_orphans( );
    
    private:
        std::unordered_map<std::uint32_t, vk_shader> shaders_;
        
        static std::uint32_t shader_id_count_;
    };
}

#endif //TWE_VK_SHADER_MANAGER_H
