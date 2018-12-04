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

#ifndef ENGINE_ENGINE_ERROR_H
#define ENGINE_ENGINE_ERROR_H

#include <system_error>

#include <vulkan/vulkan.hpp>

#include "TWE_core.h"

namespace TWE
{
    /*!
     * @brief An error for general TWE errors, inherits from
     * std::system_error
     */
    class basic_error : public std::system_error
    {
    private:
        /*!
         * @brief The category of the error. Inherits from
         * std::error_category.
         */
        class category : public std::error_category
        {
        public:
            /*!
             * @brief Get the name of the error class.
             * @return The name of the error class.
             */
            const char* TWE_API name( ) const noexcept override;
            /*!
             * @brief Get the name of the general error type.
             * @param ev The error code.
             * @return The associated name with the error code.
             */
            std::string TWE_API message( int ev ) const override;
        };

    public:
        enum class error_code : int
        {
            /* General engine errors. */
            engine_error = -1,
            
            /* General Vulkan Environment errors. */
            vk_version_error = -2,
            vk_not_supported_error = -3,
            vk_instance_ext_support_error = -4,
            vk_validation_layer_support_error = -5
        };
        
    public:
        /*!
         * @brief Ctor to create the error
         * @param code The error code.
         * @param message The message to print alongside the error.
         */
        TWE_API basic_error( const error_code& code, const std::string& message );
    };
}

#endif //ENGINE_ENGINE_ERROR_H
