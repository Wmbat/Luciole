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

#ifndef XCB_WINDOW_H
#define XCB_WINDOW_H

#if defined( VK_USE_PLATFORM_XCB_XHR )
#include <xcb/xcb.h>

#include "base_window.h"

namespace TWE
{
    class xcb_window : public base_window
    {

    };
}
#endif

#endif // XCB_WINDOW_H