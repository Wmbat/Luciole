/*!
 *  Copyright (C) 2018 BouwnLaw
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

#ifndef VULKAN_PROJECT_GAME_H
#define VULKAN_PROJECT_GAME_H

#include "../engine/renderer.h"
#include "../engine/window.h"

class game
{
public:
    explicit game( engine::window& wnd );

    void run( );

private:
    engine::window& wnd_;
    engine::renderer renderer_;

    float time_passed_ = 0;
    int frames_passed_ = 0;
};


#endif //VULKAN_PROJECT_GAME_H
