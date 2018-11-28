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

namespace TWE
{
    class basic_error : public std::system_error
    {
    private:
        class category : public std::error_category
        {
        public:
            const char* name( ) const noexcept override;
            std::string message( int ev ) const override;
        };

    public:
        enum class flags : int
        {
            engine_error = -1,
            vk_version_error = -2,
            vk_not_supported_error = -3,
            vk_instance_ext_support_error = -4,
            vk_validation_layer_support_error = -5
        };
        
    public:
        basic_error( const flags& error_flags, const std::string& message )
            : system_error( static_cast<int>( error_flags ), category( ), message )
        {
        
        }
    };
}

#endif //ENGINE_ENGINE_ERROR_H
