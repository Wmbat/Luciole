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

#ifndef LUCIOLE_SURFACE_HPP
#define LUCIOLE_SURFACE_HPP

#include "instance.hpp"
#include "utils.hpp"

#include "../window/base_window.hpp"

namespace lcl::vulkan
{
    struct surface
    {
    public:
        LUCIOLE_API surface( ) = default;
        LUCIOLE_API surface( const base_window& window, const instance& instance );
        surface( const surface& other ) = delete;
        LUCIOLE_API surface( surface&& other );
        LUCIOLE_API ~surface( );

        surface& operator=( const surface& rhs ) = delete;
        LUCIOLE_API surface& operator=( surface&& rhs );


    public:
        VkSurfaceKHR handle_ = VK_NULL_HANDLE;

    private:
        VkInstance instance_ = VK_NULL_HANDLE;
    };

} // lcl::vulkan

#endif // LUCIOLE_SURFACE_HPP