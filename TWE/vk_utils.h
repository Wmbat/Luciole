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

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT
#include <vulkan/vulkan.hpp>

#include "utilities/vk_error.h"

namespace TWE
{
#if defined( NDEBUG )
    static constexpr bool enable_debug_layers = false;
#else
    static constexpr bool enable_debug_layers = true;
#endif
    
    /**
     *  @brief Helper function to check if an object was successfully created. Throws
     *  an exception if it is not, otherwise, returns the desired object.
     *  @tparam T, The desired object type.
     *  @param return_obj, Struct containing the desired object and its creation result.
     *  @param error_msg, Message to display in case of creation failure.
     *  @return The successfully creation object.
     */
    template<typename T>
    inline const T check_vk_result_value( const vk::ResultValue<T> &return_obj, const std::string& msg  )
    {
        if( return_obj.result != vk::Result::eSuccess )
        {
            throw vk_error{ return_obj.result, msg };
        }
        else
        {
            return return_obj.value;
        }
    }
    
    /**
     *  @brief Helper func to check if an object was successfully created. Throws
     *  an exception if it is not.
     *  @param result, The result to check.
     *  @param error_msg, The message to display in case of error.
     */
    inline void check_vk_result( const vk::Result& result, const std::string& msg )
    {
        if( result != vk::Result::eSuccess )
        {
            throw vk_error{ result, msg };
        }
    }
}

#endif //VULKAN_PROJECT_VULKAN_UTILS_H
