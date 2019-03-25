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

#ifndef MARSUPIAL_GRAPHICS_MESH_HPP
#define MARSUPIAL_GRAPHICS_MESH_HPP

#include <vector>

#include "vertex.hpp"

namespace lcl
{
    struct mesh
    {
        mesh(
            const std::vector<glm::vec3>& positions = { },
            const std::vector<glm::vec4>& colours = { },
            const std::vector<uint32_t>& indices = { } )
            :
            positions_( positions ),
            colours_( colours )
        { }
        
        mesh& set_positions( const std::vector<glm::vec3>& positions )
        {
            positions_ = positions;
            return *this;
        }
        mesh& set_colours( const std::vector<glm::vec4>& colours )
        {
            colours_ = colours;
            return *this;
        }
        mesh& set_indices( const std::vector<std::uint32_t>& indices )
        {
            indices_ = indices;
            return *this;
        }
        
        std::vector<glm::vec3> positions_;
        std::vector<glm::vec4> colours_;
        std::vector<std::uint32_t> indices_;
    };
}

#endif //MARSUPIAL_GRAPHICS_MESH_HPP
