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

#ifndef VULKAN_PROJECT_VULKAN_UTILS_H
#define VULKAN_PROJECT_VULKAN_UTILS_H

#include <string>

#include <vulkan/vulkan.h>

#include "utilities/vk_error.h"

namespace TWE
{
#if defined( NDEBUG )
    static constexpr bool enable_debug_layers = false;
#else
    static constexpr bool enable_debug_layers = true;
#endif
    
    /**
     *  @brief Returns a struct holding the desired object and a bool
     *  to check whether it's creation was successful.
     *  @tparam T, The desired object type.
     */
    template<typename T>
    struct vk_return_type
    {
        VkResult result_;
        T value_;
    };
    
    /**
     *  @brief Helper function to check if an object was successfully created. Throws
     *  an exception if it is not, otherwise, returns the desired object.
     *  @tparam T, The desired object type.
     *  @param return_obj, Struct containing the desired object and its creation result.
     *  @param error_msg, Message to display in case of creation failure.
     *  @return The successfully creation object.
     */
    template<typename T>
    inline const T check_vk_return_type_result( const vk_return_type<T> &return_obj, const std::string& message )
    {
        if( return_obj.result_ != VK_SUCCESS )
        {
            throw vk_error{ return_obj.result_, message };
        }
        else
        {
            return std::move( return_obj.value_ );
        }
    }
    
    /**
     *  @brief Helper func to check if an object was successfully created. Throws
     *  an exception if it is not.
     *  @param result, The result to check.
     *  @param error_msg, The message to display in case of error.
     */
    inline void check_vk_return_result( const VkResult result, const std::string& msg )
    {
        if( result != VK_SUCCESS )
        {
            throw vk_error{ result, msg };
        }
    }
}

#endif //VULKAN_PROJECT_VULKAN_UTILS_H
