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
    /**
     *  @brief Holds the key mapping of a mouse button layout
     *  as well as the handling of the mouse button events.
     */
    class mouse
    {
    public:
        /**
         *  @brief Enum to specify what kind of event could
         *  have.
         */
        enum class type : std::uint32_t
        {
            pressed,
            released,

            invalid
        };
        /**
         *  @brief Enum to specify the mapping of the mouse button.
         */
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

        /**
         *  @brief The event used to process button events.
         */
        struct button_event
        {
            button button_ = button::invalid;
            type type_ = type::invalid;
        };

    public:
        /**
         *  @brief Get the current position of the mouse cursor.
         *  @return A vector containing the position of the cursor.
         */
        glm::i32vec2 cursor_pos( ) const noexcept;
        /**
         *  @brief Update the cursor's position.
         *  @param x, The new x position of the cursor.
         *  @param y, The new y position of the cursor.
         */
        void update_pos( int32_t x, int32_t y ) noexcept;

        /**
         *  @brief Check whether there are any events in the buffer.
         *  @return Whether the buffer is empty or not (empty = true).
         */
        bool is_button_empty( ) const noexcept;
        /**
         *  @brief Check whether a button is pressed.
         *  @param button, Which button to check for.
         *  @return Whether the button was pressed or not (pressed = true).
         */
        bool is_button_pressed( button button ) const noexcept;

        /**
         *  @brief Remove and return the element at the head of the buffer.
         *  @return The element at the head of the buffer.
         */
        button_event pop_button_event( ) noexcept;
        /**
         *  @brief Place an event at the tail of the buffer. If the buffer is
         *  full, remove the event at the head and insert the new event
         *  @param event, Event to insert in the buffer.
         */
        void emplace_button_event( const button_event& event ) noexcept;

    private:
        /**
         *  @brief The Max number of elements the buffer can hold.
         */
        static constexpr std::uint8_t MAX_BUTTON_BUFFER_SIZE_ = 32;

        /**
         *  @brief The x position of the mouse cursor.
         */
        int32_t x_pos_ = 0;
        /**
         *  @brief The y position of the mouse cursor.
         */
        int32_t y_pos_ = 0;

        /**
         *  @brief Tells the state of each key (pressed = true).
         */
        std::bitset<static_cast<size_t>( button::last )> button_state_;

        /**
         *  @brief The buffer of mouse button events.
         */
        button_event buffer_[MAX_BUTTON_BUFFER_SIZE_];

        /**
         *  @brief The position of the first element in the buffer.
         */
        size_t head_ = 0;
        /**
         *  @brief The position one past the last element in the buffer.
         */
        size_t tail_ = 0;
    };
}

#endif //VULKAN_PROJECT_MOUSE_H
