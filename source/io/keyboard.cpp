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

#include <iostream>
#include <assert.h>

#include "input_devices/keyboard.h"

namespace TWE
{
    bool keyboard::is_key_pressed( key key_code ) const noexcept
    {
        return key_states_[static_cast<size_t>( key_code )];
    }

    keyboard::key_event keyboard::pop_key_event( ) noexcept
    {
        auto event_ret = key_buffer_[head_];

        key_buffer_[head_] = key_event{ 0, event_type::invalid };

        head_ = ( head_ + 1 ) % MAX_BUFFER_SIZE_;

        return event_ret;
    }

    void keyboard::emplace_event( const key_event& event ) noexcept
    {
        if ( ( tail_ + 1 ) % MAX_BUFFER_SIZE_ == head_ )
        {
            pop_key_event();
        }

        key_buffer_[tail_] = event;

        key_states_[key_buffer_[tail_].id_] = ( key_buffer_[tail_].type_ == event_type::pressed );

        tail_ = ( tail_ + 1 ) % MAX_BUFFER_SIZE_;
    }

    bool keyboard::empty( ) const noexcept
    {
        return head_ == tail_;
    }
}
