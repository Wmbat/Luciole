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
    event base_window::event_handler::pop_event ( ) noexcept
    {
        const auto ret = buffer_[head_];

        buffer_[head_] = { };

        head_ = ( head_ + 1 ) % BUFFER_SIZE;

        return ret;
    }

    void base_window::event_handler::push_event ( event event ) noexcept
    {
        if( ( tail_ + 1 ) % BUFFER_SIZE == head_ )
        {
            pop_event ( );
        }

        buffer_[tail_] = event;

        if( buffer_[tail_].type_ == event::type::mouse_button_pressed )
        {
            button_states_[static_cast< size_t >( buffer_[tail_].mouse_button.button_ )] = true;
        }
        else if( buffer_[tail_].type_ == event::type::mouse_button_released )
        {
            button_states_[static_cast< size_t >( buffer_[tail_].mouse_button.button_ )] = false;
        }
        else if( buffer_[tail_].type_ == event::type::key_pressed )
        {
            key_states_[static_cast< size_t >( buffer_[tail_].key.key_ )] = true;
        }
        else if( buffer_[tail_].type_ == event::type::key_released )
        {
            key_states_[static_cast< size_t >( buffer_[tail_].key.key_ )] = false;
        }

        tail_ = ( tail_ + 1 ) % BUFFER_SIZE;
    }

    bool base_window::event_handler::is_keyboard_key_pressed ( keyboard::key key_code ) noexcept
    {
        return key_states_[static_cast< size_t >( key_code )];
    }

    bool base_window::event_handler::is_mouse_button_pressed ( mouse::button button_code ) noexcept
    {
        return button_states_[static_cast< size_t >( button_code )];
    }

    bool base_window::event_handler::is_empty ( ) const noexcept
    {
        return head_ == tail_;
    }
    
    event base_window::pop_event( ) noexcept
    {
        return event_handler_.pop_event();
    }
    
    bool base_window::is_event_queue_empty( )
    {
        return event_handler_.is_empty();
    }
    
    bool base_window::is_kbd_key_pressed( keyboard::key key_code ) noexcept
    {
        return event_handler_.is_keyboard_key_pressed( key_code );
    }
    
    bool base_window::is_mb_pressed( mouse::button button_code ) noexcept
    {
        return event_handler_.is_mouse_button_pressed( button_code );
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