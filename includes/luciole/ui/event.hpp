/*
 *  Copyright (C) 2018-2019 Wmbat
 *
 *  wmbat@protonmail.com
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

#ifndef LUCIOLE_WINDOW_EVENT_HPP
#define LUCIOLE_WINDOW_EVENT_HPP

#include <luciole/ui/keyboard.hpp>
#include <luciole/ui/mouse.hpp>

#include <glm/glm.hpp>

#include <luciole/utilities/delegate.hpp>

/*!
 * @brief A struct to hold the information of the key event.
 * @var code_ -> A keyboard::key enum value corresponding to which key
 * was pressed or released.
 * @var state_ -> A keyboard::key_state enum value corresponding to whether
 * the key was pressed or released
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

/*!
 * @brief Holds the information of the mouse button event.
 * @var code_ -> A mouse::button enum value corresponding to which
 * mouse button was pressed or released.
 * @var state_ -> A mouse::button_state enum value to query whether
 * the button was pressed or released.
 * @var position_ -> A glm::i32vec2 to keep track of where, in the window screen,
 * was the button pressed or released.
 */
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

/*!
 * @brief Holds the information of mouse movements on the window space.
 * @var position_ -> A glm::i32vec2 to keep track of where, in the window scree,
 * is the mouse.
 */
struct mouse_motion_event
{
    glm::i32vec2 position_;
    
    mouse_motion_event& set_position( const glm::i32vec2& position )
    {
        position_ = position;
        return *this;
    }
};

/*!
 * @brief Hold information regarding the window is closed.
 * @var is_closed_ -> A boolean value to state whether the window
 * is closed or not. Where TRUE means the window is closed and FALSE
 * means the window is still open.
 */
struct window_close_event
{
    bool is_closed_;
    
    window_close_event& set_is_closed ( const bool is_closed )
    {
        is_closed_ = is_closed;
        return *this;
    }
};

/*!
 * @brief Hold information regarding the size of the framebuffer.
 * @var size_ -> A glm::u32vec2 to hold the new size of the framebuffer.
 */
struct framebuffer_resize_event
{
    glm::u32vec2 size_;
    
    framebuffer_resize_event& set_size( const glm::u32vec2& size )
    {
        size_ = size;
        return *this;
    }
};

using key_event_delg = delegate<void( const key_event& )>;
using mouse_button_event_delg = delegate<void( const mouse_button_event& )>;
using mouse_motion_event_delg = delegate<void( const mouse_motion_event& )>;
using window_close_event_delg = delegate<void( const window_close_event& )>;
using framebuffer_resize_event_delg = delegate<void( const framebuffer_resize_event& )>;


#endif //LUCIOLE_WINDOW_EVENT_HPP
