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

#include <chrono>
#include <iostream>
#include <unordered_map>

#include "game.h"

game::game( TWE::window& wnd )
    :
    wnd_( wnd ),
    core_( wnd, wnd_.get_title(), VK_MAKE_VERSION( 0, 0, 1 ) )
    // renderer_( wnd_, "Test", VK_MAKE_VERSION( 0, 0, 1 ) )
{
    //renderer_.setup_graphics_pipeline( "test/shaders/vert.spv", "test/shaders/frag.spv" );
    //renderer_.record_command_buffers( );
}

void game::run( )
{
    auto time_point = std::chrono::steady_clock::now( );
    float max_dt = 1.0f / 20.0f;

    while( wnd_.is_open() )
    {
        wnd_.poll_events( );

        float dt;
        {
            const auto new_time_point = std::chrono::steady_clock::now( );
            dt = std::chrono::duration<float>( new_time_point - time_point ).count( );
            time_point = new_time_point;
        }
        dt = std::min( dt, max_dt );

        time_passed_ += dt;
        frames_passed_ += 1;

        if ( time_passed_ >= 1.0 )
        {
            wnd_.set_title( wnd_.get_title() + " - FPS: " + std::to_string( frames_passed_ / time_passed_ ) );

            time_passed_ = 0;
            frames_passed_ = 0;
        }

        while( !wnd_.no_window_event() )
        {
            auto e = wnd_.pop_window_event();
        }
    }

    /*
        if( !wnd_.input_devices_.event_handler_.empty() )
        {
            auto event = wnd_.input_devices_.event_handler_.pop_event( );

            wnd_.handle_event( event );

            if( event.type_ == engine::window::event_handler::event::type::window_resize ||
                event.type_ == engine::window::event_handler::event::type::framebuffer_resize )
            {
                renderer_.handle_resize( "test/shaders/vert.spv", "test/shaders/frag.spv", event.x_, event.y_ );
            }
        }


        if( wnd_.input_devices_.keyboard_.is_key_pressed( GLFW_KEY_ESCAPE ) )
        {
            is_open = false;
        }

        renderer_.draw_frame( "test/shaders/vert.spv", "test/shaders/frag.spv" );
    }

     */
}