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

#include "keyboard.h"
#include "mouse.h"

namespace TWE
{
    /*!
     * @brief An event struct that holds the data of a key event.
     */
    struct key_event
    {
        keyboard::key code_;
        keyboard::key_state state_;
        
        key_event& set_code( const keyboard::key key_code )
        {
            code_ = key_code;
            return *this;
        }
        key_event& set_state( const keyboard::key_state key_state )
        {
            state_ = key_state;
            return *this;
        }
    };
    
    struct mouse_button_event
    {
        mouse::button code_;
        mouse::button_state state_;
        glm::i32vec2 position_;
        
        mouse_button_event& set_code( const mouse::button button_code )
        {
            code_ = button_code;
            return *this;
        }
        mouse_button_event& set_state( const mouse::button_state button_state )
        {
            state_ = button_state;
            return *this;
        }
        mouse_button_event& set_position( const glm::i32vec2& position )
        {
            position_ = position;
            return *this;
        }
    };
    
    struct mouse_motion_event
    {
        glm::i32vec2 position_;
        
        mouse_motion_event& set_position( const glm::i32vec2& position )
        {
            position_ = position;
            return *this;
        }
    };

    struct window_close_event
    {
        bool is_closed_;

        window_close_event& set_is_closed ( const bool is_closed )
        {
            is_closed_ = is_closed;
            return *this;
        }
    };
    
    struct framebuffer_resize_event
    {
        glm::u32vec2 size_;
        
        framebuffer_resize_event& set_size( const glm::u32vec2& size )
        {
            size_ = size;
            return *this;
        }
    };
}

#endif //TWE_EVENT_H
