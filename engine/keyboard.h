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

#ifndef VULKAN_PROJECT_KEYBOARD_H
#define VULKAN_PROJECT_KEYBOARD_H

#include <cstdint>

#include <glfw/glfw3.h>
#include <bitset>

namespace engine
{
    class keyboard
    {
    public:
        enum class type : std::uint32_t
        {
            pressed,
            released,
            invalid
        };

        struct key_event
        {
            std::int32_t id_ = -1;
            type type_ = type::invalid;
        };

    public:
        keyboard( );

        bool is_key_pressed( std::int32_t key_code ) const noexcept;

        void push_key_event( const key_event& event );

        bool empty( ) const noexcept;

    private:
        void pop_key_event( );

    private:
        static constexpr std::uint8_t MAX_BUFFER_SIZE_ = 8;

        std::bitset<GLFW_KEY_LAST> key_states_;

        key_event key_buffer_[MAX_BUFFER_SIZE_];
        int num_buffer_;

        size_t head_;
        size_t tail_;
    };
}

#endif //VULKAN_PROJECT_KEYBOARD_H
