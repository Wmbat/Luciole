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

#include "window/base_window.h"

namespace TWE
{
    
    void base_window::add_key_listener( const key_event_delg & delg )
    {
        key_event_.add_listener( delg );
    }
    void base_window::add_mouse_button_listener( const mouse_button_event_delg& delg )
    {
        mouse_button_event_.add_listener( delg );
    }
    
    void base_window::add_mouse_motion_listener( const mouse_motion_event_delg& delg )
    {
        mouse_motion_event_.add_listener( delg );
    }
    
    void base_window::add_window_close_listener( const window_close_event_delg& delg )
    {
        window_close_event_.add_listener( delg );
    }
    
    void base_window::add_framebuffer_resize_listener( const framebuffer_resize_event_delg& delg )
    {
        frame_buffer_resize_event_.add_listener( delg );
    }
    
    bool base_window::is_open( ) const noexcept
    {
        return open_;
    }
    
    uint32_t base_window::get_width( ) const noexcept
    {
        return settings_.width_;
    }
    
    uint32_t base_window::get_height( ) const noexcept
    {
        return settings_.height_;
    }
    
}