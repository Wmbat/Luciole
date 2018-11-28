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

#include <assert.h>
#include <glm/fwd.hpp>
#include <iostream>

#include "input_devices/mouse.h"

namespace TWE
{
    bool mouse::is_button_empty( ) const noexcept
    {
        return head_ == tail_;
    }

    bool mouse::is_button_pressed( button button ) const noexcept
    {
        return button_state_[static_cast<size_t>( button )];
    }

    glm::i32vec2 mouse::cursor_pos( ) const noexcept
    {
        return { x_pos_, y_pos_ };
    }

    void mouse::update_pos( int32_t x, int32_t y ) noexcept
    {
        x_pos_ = x;
        y_pos_ = y;
    }

    mouse::button_event mouse::pop_button_event( ) noexcept
    {
        auto ret = buffer_[head_];

        buffer_[head_] = button_event{ };

        head_ = ( head_ + 1 ) % MAX_BUTTON_BUFFER_SIZE_;

        return ret;
    }

    void mouse::emplace_button_event( const mouse::button_event &event ) noexcept
    {
        if( ( tail_ + 1 ) % MAX_BUTTON_BUFFER_SIZE_ == head_ )
        {
            pop_button_event();
        }

        buffer_[tail_] = event;

        if( buffer_[tail_].type_ == type::pressed )
        {
            button_state_[static_cast<size_t>( buffer_[tail_].button_ )] = true;
        }
        else if( buffer_[tail_].type_ == type::released )
        {
            button_state_[static_cast<size_t>( buffer_[tail_].button_ )] = false;
        }

        tail_ = ( tail_ + 1 ) % MAX_BUTTON_BUFFER_SIZE_;
    }
}