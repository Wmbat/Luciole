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

#ifndef ENGINE_VK_ERROR_H
#define ENGINE_VK_ERROR_H

#include <system_error>

#include <vulkan/vulkan.h>

namespace TWE
{
    class vk_error : public std::system_error
    {
    private:
        class category;
        
    public:
        vk_error( const VkResult& result, const std::string& message )
            : system_error( result, category( ), message )
        { }
        
    private:
        class category : public std::error_category
        {
        public:
            const char* name( ) const noexcept override;
            std::string message( int ev ) const override;
        };
    };
}

#endif //ENGINE_vk_error_H
