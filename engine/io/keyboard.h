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

#ifndef VULKAN_PROJECT_KEYBOARD_H
#define VULKAN_PROJECT_KEYBOARD_H

#include <cstdint>

#include <glfw/glfw3.h>
#include <bitset>

#include "keyboard_layouts.h"

namespace TWE
{
    class keyboard
    {
    public:
        enum class layout : std::uint32_t
        {
            us_qwerty,
            us_dvorak
        };

        enum class event_type : std::uint32_t
        {
            pressed,
            released,
            invalid
        };

        struct key_event
        {
            std::int32_t id_ = -1;
            event_type type_ = event_type::invalid;
        };

    public:
        keyboard( );

        // maybe, in the future.
        void set_layout( layout layout );

        bool is_key_pressed( std::int32_t key_code ) const noexcept;
        bool empty( ) const noexcept;

        key_event pop_key_event( );
        void emplace_event( const key_event& event );


    private:
        static constexpr std::uint8_t MAX_BUFFER_SIZE_ = 8;

        std::bitset<key_codes::max_keys> key_states_;

        key_event key_buffer_[MAX_BUFFER_SIZE_];
        int num_buffer_;

        size_t head_;
        size_t tail_;
    };
}

#endif //VULKAN_PROJECT_KEYBOARD_H
