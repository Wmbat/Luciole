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

#ifndef TWE_TWE_H
#define TWE_TWE_H

#include "window.h"
#include "vulkan_utils.h"
#include "renderer.h"
#include "log.h"
#include "utilities/file_io.h"
#include "input_devices/mouse.h"
#include "input_devices/keyboard.h"

namespace TWE
{
    inline static void init_engine( )
    {
        log::init();
    }
}


#endif //TWE_TWE_H
