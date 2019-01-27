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

#ifndef ENGINE_PIPELINE_LAYOUT_HPP
#define ENGINE_PIPELINE_LAYOUT_HPP

#include "core.hpp"

#include "../twe_core.hpp"

namespace twe::vulkan
{
    class pipeline_layout
    {
    public:
        pipeline_layout( ) = default;
        
    private:
        vk::UniquePipelineLayout layout_;
    };
}

#endif //ENGINE_PIPELINE_LAYOUT_HPP
