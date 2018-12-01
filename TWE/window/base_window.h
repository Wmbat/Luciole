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

#ifndef BASE_WINDOW_H
#define BASE_WINDOW_H

#include "../TWE_core.h"
#include "../event.h"

namespace TWE
{
    class base_window
    {
    public:
        class event_handler
        {
        public:
            event TWE_API pop_event ( ) noexcept;

            void TWE_API push_event ( event event ) noexcept;

            bool TWE_API is_keyboard_key_pressed ( keyboard::key key_code ) noexcept;
            bool TWE_API is_mouse_button_pressed ( mouse::button button_code ) noexcept;

            bool TWE_API is_empty ( ) const noexcept;

        private:
            static constexpr uint16_t BUFFER_SIZE = 64;

            std::bitset<static_cast< size_t >( keyboard::key::last )> key_states_;
            std::bitset<static_cast< size_t >( mouse::button::last )> button_states_;

            event buffer_[BUFFER_SIZE];
            uint32_t head_ = 0;
            uint32_t tail_ = 0;
        };

    private:

    };
}

#endif //BASE_WINDOW_H