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

#ifndef VULKAN_PROJECT_WINDOW_H
#define VULKAN_PROJECT_WINDOW_H

#include <vulkan/vulkan.h>

#if defined( _WIN32 )
#include <window.h>
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
#include <wayland-client.h>
#elif defined( VK_USE_PLATFORM_XCB_KHR )
#include <xcb/xcb.h>
#endif

#include <vector>

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

#include "vulkan_utils.h"
#include "keyboard.h"
#include "mouse.h"

namespace engine
{
    class window
    {
    public:
        class event_handler
        {
        public:
            struct event
            {
                enum class type : std::int32_t
                {
                    window_move,
                    window_resize,
                    framebuffer_resize,
                    invalid
                };

                type type_ = type::invalid;
                std::int32_t x_ = 0;
                std::int32_t y_ = 0;
            };

        public:
            event_handler( );

            event pop_event( );
            void push_event( const event& e );

            bool empty( ) const noexcept;

        private:
            static constexpr int MAX_EVENTS = 16;

            event event_buffer_[MAX_EVENTS];
            size_t num_events_pending_;

            size_t head_;
            size_t tail_;
        };

    public:
        window( ) = default;
        window( uint32_t width, uint32_t height, const std::string& title );
        window( const window& other ) = delete;
        window( window&& other ) noexcept;
        ~window( );

        window& operator=( const window& other ) = delete;
        window& operator=( window&& other ) noexcept;

        bool is_open( );

        void poll_events( );
        void handle_event( const event_handler::event& e );

        std::vector<const char*> get_required_extensions( ) const noexcept;
        vk_return_obj<VkSurfaceKHR> create_surface( const VkInstance& instance ) const noexcept;

        const uint32_t get_width( ) const noexcept;
        const uint32_t get_height( ) const noexcept;

    private:
        GLFWwindow* p_glfw_window_ = nullptr;

        std::string title_;
        uint32_t width_ = 0;
        uint32_t height_ = 0;
        uint32_t x_pos_ = 100;
        uint32_t y_pos_ = 100;

    public:
        struct input_devices
        {
            keyboard keyboard_ = keyboard( );
            mouse mouse_ = mouse( );
            event_handler event_handler_ = event_handler( );
        } input_devices_;
    };
}

#endif //VULKAN_PROJECT_WINDOW_H
