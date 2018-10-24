/*!
 *  Copyright (C) 2018 BouwnLaw
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

#ifndef VULKAN_PROJECT_VULKAN_UTILS_H
#define VULKAN_PROJECT_VULKAN_UTILS_H

#include <string>
#include <stdexcept>

#include <vulkan/vulkan.h>

namespace engine
{
#ifdef NDEBUG
    static constexpr bool enable_validation_layers = false;
#else
    static constexpr bool enable_debug_layers = true;
#endif

    template<typename T>
    struct vk_return_obj
    {
        VkResult result_;
        T value_;
    };

    template<typename T>
    const T check_vk_return_state( const vk_return_obj<T>& return_obj, const std::string& error_msg )
    {
        if( return_obj.result_ != VK_SUCCESS )
        {
            throw std::runtime_error{ error_msg };
        }
        else
        {
            return std::move( return_obj.value_ );
        }
    }

    inline void check_vk_return_result( const VkResult result, const std::string& error_msg )
    {
        if( result != VK_SUCCESS )
        {
            throw std::runtime_error{ error_msg };
        }
    }
}

#endif //VULKAN_PROJECT_VULKAN_UTILS_H
