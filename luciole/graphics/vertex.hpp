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


#ifndef LUCIOLE_GRAPHICS_VERTEX_HPP
#define LUCIOLE_GRAPHICS_VERTEX_HPP

#include <glm/glm.hpp>

namespace lcl
{
    struct vertex
    {
        glm::vec3 position_;
        glm::vec4 colour_;
    };
    
    /*
    struct vertex_input_config
    {
        std::vector<vk::VertexInputBindingDescription> bindings_;
        std::vector<vk::VertexInputAttributeDescription> attributes_;
    };
    */
}

#endif //LUCIOLE_GRAPHICS_VERTEX_HPP
