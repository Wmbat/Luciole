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
     * @brief An event that hold key press information with
     * the associated key code.
     */
    struct key_press_event
    {
        keyboard::key key_code_;
    
        key_press_event& set_key_code( const keyboard::key key_code )
        {
            key_code_ = key_code;
            return *this;
        }
    };
    /*!
     * @brief An event that hold key released information with
     * the associated key code.
     */
    struct key_release_event
    {
        keyboard::key key_code_;
        
        key_release_event& set_key_code( const keyboard::key key_code )
        {
            key_code_ = key_code;
            return *this;
        }
    };
    
    /*!
     * @brief An event that hold mouse button press information
     * with the associated button code.
     */
    struct mouse_button_press_event
    {
        mouse::button button_code_;
        
        mouse_button_press_event& set_button_code( const mouse::button button_code )
        {
            button_code_ = button_code;
            return *this;
        }
    };
    
    /*!
     * @brief An event that hold mouse button release information
     * with the associated button code.
     */
    struct mouse_button_release_event
    {
        mouse::button button_code_;
        
        mouse_button_release_event& set_button_code( const mouse::button button_code )
        {
            button_code_ = button_code;
            return *this;
        }
    };
    
    /*!
     * @brief An event that hold mouse position information.
     */
    struct mouse_motion_event
    {
        int32_t x_;
        int32_t y_;
        
        mouse_motion_event& set_position( const int32_t x, const int32_t y )
        {
            x_ = x;
            y_ = y;
            return *this;
        }
    };
}

#endif //TWE_EVENT_H
