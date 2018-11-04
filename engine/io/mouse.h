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

#ifndef VULKAN_PROJECT_MOUSE_H
#define VULKAN_PROJECT_MOUSE_H

#include <bitset>
#include <cstdint>
#include <cstddef>
#include <queue>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace TWE
{
    class mouse
    {
    public:
        enum class type : std::uint32_t
        {
            pressed,
            released,

            invalid
        };

        enum class button : std::int32_t
        {
            invalid = -1,

            l_click = 1,
            scroll_click = 2,
            r_click = 3,
            scroll_up = 4,
            scroll_down = 5,
            scroll_left = 6,
            scroll_right = 7,
            side_button_1 = 8,
            side_button_2 = 9,

            last = 16
        };

        struct button_event
        {
            button button_ = button::invalid;
            type type_ = type::invalid;
        };

    public:
        mouse( );

        glm::i32vec2 cursor_pos( ) const noexcept;
        void update_pos( int32_t x, int32_t y ) noexcept;

        bool is_button_empty( ) const noexcept;
        bool is_button_pressed( button button ) const noexcept;

        button_event pop_button_event( );
        void emplace_button_event( const button_event& event );

    private:
        static constexpr std::uint8_t MAX_BUTTON_BUFFER_SIZE_ = 8;

        int32_t x_pos_ = 0;
        int32_t y_pos_ = 0;

        std::bitset<static_cast<size_t>( button::last )> button_state_;

        button_event button_buffer_[MAX_BUTTON_BUFFER_SIZE_];
        size_t num_button_buffer_;

        size_t button_head_;
        size_t button_tail_;
    };
}

#endif //VULKAN_PROJECT_MOUSE_H
