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

#include <chrono>
#include <iostream>
#include <unordered_map>

#include "game.h"

game::game( engine::window& wnd )
    :
    wnd_( wnd ),
    renderer_( wnd_, "Test", VK_MAKE_VERSION( 0, 0, 1 ) )
{
    renderer_.setup_graphics_pipeline( "test/shaders/vert.spv", "test/shaders/frag.spv" );
    renderer_.record_command_buffers( );
}

void game::run( )
{
    auto time_point = std::chrono::steady_clock::now( );
    float max_dt = 1.0f / 20.0f;

    bool is_open = wnd_.is_open();
    while( is_open )
    {
        wnd_.poll_events();

        float dt;
        {
            const auto new_time_point = std::chrono::steady_clock::now( );
            dt = std::chrono::duration<float>( new_time_point - time_point ).count( );
            time_point = new_time_point;
        }
        dt = std::min( dt, max_dt );

        time_passed_ += dt;
        frames_passed_ += 1;

        if( time_passed_ >= 0.1 )
        {
            // std::cout << "FPS: " << frames_passed_ / time_passed_ << "\n";

            time_passed_ = 0;
            frames_passed_ = 0;
        }

        if( wnd_.p_keyboard_->is_key_pressed( GLFW_KEY_ESCAPE ) )
        {
            is_open = false;
        }

        renderer_.draw_frame();
    }
}
