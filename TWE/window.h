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

#ifndef VULKAN_PROJECT_WINDOW_H
#define VULKAN_PROJECT_WINDOW_H

#include <vector>
#include <memory>
#include <iostream>
#include <functional>

#if defined( _WIN32 )
#include <window.h>
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
#include <wayland-client.h>
#elif defined( VK_USE_PLATFORM_XCB_KHR )
#include <xcb/xcb.h>
#endif

#include <vulkan/vulkan.h>

#include "vulkan_utils.h"

#include "event.h"

namespace TWE
{
    /**
     *  @brief Creates a cross-platform window between GNU/linux
     *  and Windows. It offers a way to handle all events
     *  from the window, keyboard and mouse as well as some
     *  customization.
     *  The window class is move only.
     */
    class window
    {
    public:
        /**
         *  @brief Handles window events.
         */
        class event_handler
        {
        public:
            event_handler( );
            
            bool is_empty( ) const noexcept;
            
            event pop_event( ) noexcept;
            
            void emplace_event( event event ) noexcept;

        private:
            static constexpr uint16_t BUFFER_SIZE = 64;
            
            event buffer_[BUFFER_SIZE];
            
            uint32_t num_elem_;

            uint32_t head_;
            uint32_t tail_;
        };

    public:
        explicit window( const std::string& title );
        window( const window& rhs ) noexcept = delete;
        window( window&& rhs ) noexcept;
        ~window( );

        void poll_events( );

        void set_title( const std::string& title ) noexcept;
        
        const std::string& get_title( ) const noexcept;

        bool is_open( ) const noexcept;

        void handle_event( const event_handler::event& event ) noexcept;

        /**
         *  @brief Create a VkSurfaceKHR object from the window.
         *  @param instance, The Vulkan instance used to create the surface.
         *  @return The Surface and its creation result.
         */
        vk_return_type<VkSurfaceKHR> create_surface( const VkInstance& instance ) const noexcept;

///////////////////////////////// Window Event //////////////////////////////////

        /**
         *  @brief Query whether the window event queue is empty.
         *  @return Whether the window event queue is empty or not (empty = true).
         */
        bool no_window_event( ) const noexcept;

        /**
         *  @brief Retrieve the first event from the window event handler.
         *  @return The event at the head of the event queue.
         */
        event_handler::event pop_window_event( ) noexcept;

/////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// Mouse Event //////////////////////////////////

        /**
         *  @brief Query whether the mouse event queue is empty.
         *  @return Whether the mouse button queue is empty (empty = true).
         */
        bool no_button_event( ) const noexcept;
        /**
         *  @brief Query whether a mouse button is currently pressed.
         *  @param button, The button to check.
         *  @return Whether the button is pressed or not.
         */
        bool is_button_pressed( mouse::button button ) const noexcept;

        /**
         *  @brief Retrieve the first event from the mouse event queue.
         *  @return The mouse button event at the head of the queue.
         */
        mouse::button_event pop_button_event( ) noexcept;

        /**
         *  @brief Get the mouse cursor's current position.
         *  @return The position of the cursor.
         */
        glm::i32vec2 cursor_position( ) const noexcept;

/////////////////////////////////////////////////////////////////////////////////


//////////////////////////////// Keyboard Event /////////////////////////////////

        /**
         *  @brief Query whether the keyboard event queue is empty.
         *  @return Whether the keyboard key queue is empty (empty = true).
         */
        bool no_key_event( );
        /**
         *  @brief Query whether a key is currently being pressed.
         *  @param key_code, The Key to check.
         *  @return Whether the key was pressed or not.
         */
        bool is_key_pressed( keyboard::key key_code ) const noexcept;

        /**
         * @brief Retrieve the first event from the keyboard event queue.
         * @return The keyboard key event at the head of the queue.
         */
        keyboard::key_event pop_key_event( );

/////////////////////////////////////////////////////////////////////////////////

        /**
         *  @brief Disabled copy assignment.
         */
        window& operator=( const window& rhs ) noexcept = delete;
        /**
         *  @brief Enabled move assignment.
         */
        window& operator=( window&& rhs ) noexcept;

        /**
         * @brief Get the width of the window.
         * @return The width of the window.
         */
        uint32_t get_width( ) const noexcept;
        /**
         * @brief Get the height of the window.
         * @return The height of the window.
         */
        uint32_t get_height( ) const noexcept;

    private:
        /**
         *  @brief Title of the window.
         */
        std::string title_;

        /**
         *  @brief States whether the window is open or not.
         */
        bool open_ = false;

#if defined( _WIN32 )

#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )

#elif defined( VK_USE_PLATFORM_XCB_KHR )
        /**
         *  @brief Pointer to a xcb_connection_t type.
         */
        std::unique_ptr<xcb_connection_t, std::function<void( xcb_connection_t* )>> p_xcb_connection_;
        /**
         *  @brief Pointer to a xcb_screen_t type.
         */
        xcb_screen_t* p_xcb_screen_;
        /**
         *  @brief Instance of a xcb_window_t type.
         */
        xcb_window_t xcb_window_;

        /**
         *  @brief Pointer to a xcb_intern_atom_reply_t type, used
         *  to detect window destruction event.
         */
        std::unique_ptr<xcb_intern_atom_reply_t> p_xcb_wm_delete_window_;
#endif
        /**
         *  @brief Helps handle window events.
         */
        event_handler event_handler_;

        /**
         *  @brief organize the window settings.
         */
        struct settings
        {
            /**
             *  @brief The window position, from the top left corner.
             */
            uint32_t x_position = 0;
            uint32_t y_position = 0;

            /**
             *  @brief The window size.
             */
            uint32_t width_ = 1080;
            uint32_t height_ = 720;

            /**
             *  @brief ID of the default monitor.
             */
            int default_screen_id_ = 0;

            /**
             *  @brief Set whether the window is in fullscreen or not.
             */
            bool fullscreen_ = false;
        } settings_;
    };
}

#endif //VULKAN_PROJECT_WINDOW_H
