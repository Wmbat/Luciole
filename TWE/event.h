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

#ifndef TWE_EVENT_H
#define TWE_EVENT_H

#include "input_devices/keyboard.h"
#include "input_devices/mouse.h"

namespace TWE
{
    struct event
    {
        struct key_event
        {
            keyboard::key key_;
        };
    
        struct mouse_button_event
        {
            mouse::button button_;
        };
        
        struct mouse_move_event
        {
            int32_t x_;
            int32_t y_;
        };
        
        struct window_move_event
        {
            uint32_t x_;
            uint32_t y_;
        };
        
        struct window_resize_event
        {
            uint32_t x_;
            uint32_t y_;
        };
        
        enum class type
        {
            invalid,
            
            key_pressed,
            key_released,
            
            mouse_button_pressed,
            mouse_button_released,
            mouse_move,
            
            window_focus_out,
            window_focus_in,
            window_move,
            window_resize,
        };
        
        union
        {
            key_event key;
            
            mouse_button_event mouse_button;
            mouse_move_event mouse_move;
            
            window_move_event window_move;
            window_resize_event window_resize;
        };

        type type_ = type::invalid;

        event& set_type( type type ) { type_ = type; };
        event& set_key( keyboard::key key_code ) { key.key_ = key_code; };
        event& set_mouse_button( mouse::button button ) { mouse_button.button_ = button; };
        event& set_mouse_move( int32_t x, int32_t y ) { mouse_move.x_ = x; mouse_move.y_ = y; };
        event& set_window_move( uint32_t x, uint32_t y ) { window_move.x_ = x; window_move.y_ = y; };

    };
}

#endif //TWE_EVENT_H
