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

#ifndef WIN32_WINDOW_H
#define WIN32_WINDOW_H

#if defined( VK_USE_PLATFORM_WIN32_KHR )
#include <window.h>

#include "base_window.h"

namespace TWE
{
    class win32_window : public base_window
    {

    };
}
#endif

#endif // WIN32_WINDOW_H