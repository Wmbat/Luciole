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

#include <iostream>
#include <assert.h>

#include "keyboard.h"

namespace engine
{
    keyboard::keyboard( )
        :
        num_buffer_( 0 ),
        head_( 0 ),
        tail_( 0 )
    {
    }

    bool keyboard::is_key_pressed( std::int32_t key_code ) const noexcept
    {
        return key_states_[key_code];
    }

    void keyboard::pop_key_event( )
    {
        assert( num_buffer_ >= 0 );

        --num_buffer_;
        key_buffer_[head_] = key_event{ 0, type::invalid };

        head_ = ( head_ + 1 ) % MAX_BUFFER_SIZE_;
    }

    void keyboard::push_key_event( const key_event& event )
    {
        if( num_buffer_ >= MAX_BUFFER_SIZE_ )
        {
            pop_key_event();
        }

        key_buffer_[tail_] = event;

        key_states_[key_buffer_[tail_].id_] = ( key_buffer_[tail_].type_ == type::pressed ) ? true : false;

        ++num_buffer_;
        tail_ = ( tail_ + 1 ) % MAX_BUFFER_SIZE_;
    }

    bool keyboard::empty( ) const noexcept
    {
        return num_buffer_ == 0;
    }
}
