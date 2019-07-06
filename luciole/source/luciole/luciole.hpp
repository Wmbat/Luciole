/*!
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


#ifndef LUCIOLE_LUCIOLE_HPP
#define LUCIOLE_LUCIOLE_HPP

#include "window/base_window.hpp"

#if defined( VK_USE_PLATFORM_XCB_KHR )
#include "window/xcb_window.hpp"
#endif


namespace lcl
{
    std::unique_ptr<lcl::base_window> create_window( const std::string& title )
    {
#if defined( VK_USE_PLATFORM_XCB_KHR )
        return std::make_unique<lcl::xcb_window>( title );
#endif

        return nullptr;
    }
}

#endif //LUCIOLE_LUCIOLE_HPP
