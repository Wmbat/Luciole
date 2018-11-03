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

#include <vulkan/vulkan.h>

#if defined( _WIN32 )
#include <window.h>
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
#include <wayland-client.h>
#elif defined( VK_USE_PLATFORM_XCB_KHR )
#include <xcb/xcb.h>
#endif

#include "vulkan_utils.h"
#include "io/keyboard.h"
#include "io/mouse.h"

namespace engine
{
    class window
    {
    public:
        class event_handler
        {
        public:
            enum class type : std::uint32_t
            {
                window_focus_out,
                window_focus_in,
                window_move,
                window_resize,
                invalid
            };

            struct event
            {
                type type_ = type::invalid;
                std::uint32_t x_ = 0;
                std::uint32_t y_ = 0;
            };

        public:
            event_handler( );

            bool is_empty( ) const noexcept;

            event pop_event( ) noexcept;
            void emplace_event( event event ) noexcept;

        private:
            static constexpr uint16_t BUFFER_SIZE = 16;

            event buffer_[BUFFER_SIZE];

            uint32_t num_elem_;
            uint32_t head_;
            uint32_t tail_;
        };

        explicit window( const std::string& title );
        window( const window& rhs ) noexcept = delete;
        window( window&& rhs ) noexcept;
        ~window( );

        void poll_events( );

        void set_title( const std::string& title ) noexcept;
        const std::string& get_title( ) const noexcept;

        bool is_open( ) const noexcept;

        vk_return_obj<VkSurfaceKHR> create_surface( const VkInstance& instance ) const noexcept;

/// Window Event ///
        bool no_window_event( ) const noexcept;

        event_handler::event pop_window_event( ) noexcept;

        void handle_event( const event_handler::event& event ) noexcept;
////////////////////


/// Mouse IO ///
        bool no_button_event( ) const noexcept;
        bool is_button_pressed( mouse::button button ) const noexcept;

        mouse::button_event pop_button_event( );

        glm::i32vec2 cursor_position( ) noexcept;
////////////////


/// Keyboard IO ///
        bool no_key_event( );
        bool is_key_pressed( std::int32_t key_code ) const noexcept;

        keyboard::key_event pop_key_event( );
///////////////////

        window& operator=( const window& rhs ) noexcept = delete;
        window& operator=( window&& rhs ) noexcept;

    private:
        std::string title_;
        bool open_ = false;

#if defined( _WIN32 )

#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )

#elif defined( VK_USE_PLATFORM_XCB_KHR )
        std::unique_ptr<xcb_connection_t, std::function<void( xcb_connection_t* )>> p_xcb_connection_;
        xcb_screen_t* p_xcb_screen_;
        xcb_window_t xcb_window_;

        std::unique_ptr<xcb_intern_atom_reply_t> p_xcb_wm_delete_window_;
#endif
        event_handler event_handler_;
        keyboard keyboard_;
        mouse mouse_;

        struct settings
        {
            uint32_t x_position = 0;
            uint32_t y_position = 0;

            uint32_t width_ = 1080;
            uint32_t height_ = 720;

            int default_screen_id_ = 0;

            bool fullscreen_ = false;
        } settings_;
    };
}

#endif //VULKAN_PROJECT_WINDOW_H
