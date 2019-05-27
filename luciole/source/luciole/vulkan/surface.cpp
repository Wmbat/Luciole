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

#include "surface.hpp"

namespace lcl::vulkan
{
    surface::surface( const base_window& window, const instance& instance )
        :
        instance_( instance.handle_ )
    {
        handle_ = window.create_surface( instance.handle_ );
    }
    surface::surface( surface&& other )
    {
        *this = std::move( other );
    }
    surface::~surface( )
    {
        if ( handle_ != VK_NULL_HANDLE )
        {
            vkDestroySurfaceKHR( instance_, handle_, nullptr );
        }
    }

    surface& surface::operator=( surface&& rhs )
    {
        if ( this != &rhs )
        {
            handle_ = rhs.handle_;
            rhs.handle_ = VK_NULL_HANDLE;

            instance_ = rhs.instance_;
            rhs.instance_ = VK_NULL_HANDLE;
        }
    }

} // lcl::vulkan