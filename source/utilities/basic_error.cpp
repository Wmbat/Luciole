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
 
#include "utilities/basic_error.h"

namespace TWE
{
    const char* basic_error::category::name( ) const noexcept
    {
        return "TWE Error";
    }
    
    std::string basic_error::category::message( int ev ) const
    {
        switch ( ev )
        {
        case static_cast<int>( flags::engine_error ):
            return "ENGINE_ERROR";
        case static_cast<int>( flags::vk_version_error ):
            return "VULKAN_VERSION_ERROR";
        case static_cast<int>( flags::vk_not_supported_error ):
            return "VULKAN_NOT_SUPPORTED_ERROR";
        case static_cast<int>( flags::vk_instance_ext_support_error ):
            return "VULKAN_INSTANCE_EXTENTION_SUPPORT_ERROR";
        case static_cast<int>( flags::vk_validation_layer_support_error ):
            return "VULKAN_VALIDATION_LAYERS_SUPPORT_ERROR";
        default:
            return "ENGINE_UNKNOWN_ERROR";
        }
    }
}


