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

namespace marsupial
{
    struct mesh
    {
        std::vector<vertex> vertices_;
        std::vector<uint32_t> indices_;
    };
}

#endif //MARSUPIAL_GRAPHICS_MESH_HPP
