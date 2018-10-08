//
// Created by ambre on 07/10/18.
//

#include <assert.h>
#include <glm/fwd.hpp>

#include "mouse.h"

namespace engine
{
    mouse::mouse( )
        :
        button_head_( 0 ),
        button_tail_( 0 ),
        num_button_buffer_( 0 ),
        cursor_head_( 0 ),
        cursor_tail_( 0 ),
        num_cursor_buffer_( 0 )
    {

    }

    bool mouse::is_button_pressed( int button_id ) const noexcept
    {
        assert( button_id <= GLFW_MOUSE_BUTTON_LAST );
        assert( button_id >= 0 );

        return button_state_[button_id];
    }

    glm::dvec2 mouse::cursor_pos( ) noexcept
    {
        auto top_event = cursor_buffer_[cursor_head_];

        pop_cursor_event();

        return { top_event.x_pos_, top_event.y_pos_ };
    }

    void mouse::push_button_event( const engine::mouse::button_event &event )
    {
        if( num_button_buffer_>= MAX_BUTTON_BUFFER_SIZE_ )
        {
            pop_button_event();
        }

        button_buffer_[button_tail_] = event;

        if( button_buffer_[button_tail_].type_ == type::pressed )
        {
            button_state_[button_buffer_[button_tail_].button_id_] = true;
        }
        else if( button_buffer_[button_tail_].type_ == type::released )
        {
            button_state_[button_buffer_[button_tail_].button_id_] = false;
        }

        ++num_button_buffer_;
        button_tail_ = ( button_tail_ + 1 ) % MAX_BUTTON_BUFFER_SIZE_;
    }
    void mouse::push_cursor_event( const engine::mouse::cursor_event &event )
    {
        if( num_cursor_buffer_ >= MAX_CURSOR_BUFFER_SIZE_ )
        {
            pop_cursor_event();
        }

        cursor_buffer_[cursor_tail_] = event;

        ++num_cursor_buffer_;
        cursor_tail_ = ( cursor_tail_ + 1 ) % MAX_CURSOR_BUFFER_SIZE_;
    }

    void mouse::pop_button_event( )
    {
        assert( num_button_buffer_ >= 0 );

        --num_button_buffer_;
        button_buffer_[button_head_] = button_event{ };

        button_head_ = ( button_head_ + 1 ) % MAX_BUTTON_BUFFER_SIZE_;
    }
    void mouse::pop_cursor_event( )
    {
        assert( num_cursor_buffer_ >= 0 );

        --num_cursor_buffer_;
        cursor_buffer_[cursor_head_] = cursor_event{ };

        cursor_head_ = ( cursor_head_ + 1 ) % MAX_CURSOR_BUFFER_SIZE_;
    }
}