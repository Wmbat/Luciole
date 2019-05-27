/*
 * @author wmbat@protonmail.com
 *
 * Copyright (C) 2018-2019 Wmbat
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * You should have received a copy of the GNU General Public License
 * GNU General Public License for more details.
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LUCIOLE_LAYERS_HPP
#define LUCIOLE_LAYERS_HPP

#include "utils.hpp"

namespace lcl::vulkan
{
    enum class layer_mode
    {
        e_required,
        e_optional,
        e_disabled
    };

    struct layer
    {
        const char* name_ = nullptr;
        layer_mode mode_ = layer_mode::e_disabled;
        bool enabled_ = false;
    };

    struct instance_layers
    {
        std::vector<const char*> get_enabled_layers( ) const
        {
            std::vector<const char*> layers;

            if ( lunarg_standard_validation_.enabled_ )
                layers.emplace_back( lunarg_standard_validation_.name_ );

            return layers;
        }

        void enable( const char* name )
        {
            if ( strcmp( lunarg_standard_validation_.name_, name ) == 0 && lunarg_standard_validation_.mode_ != layer_mode::e_disabled )
                lunarg_standard_validation_.enabled_ = true;
        }

        layer lunarg_standard_validation_{ "VK_LAYER_LUNARG_standard_validation", layer_mode::e_optional };
    };
}

#endif // LUCIOLE_LAYERS_HPP