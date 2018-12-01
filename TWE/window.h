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

#if defined( VK_USE_PLATFORM_WIN32_KHR )
#include <window.h>
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
#include <wayland-client.h>
#elif defined( VK_USE_PLATFORM_XCB_KHR )
#include <xcb/xcb.h>
#endif

#include <vulkan/vulkan.h>

#include "TWE_core.h"
#include "vulkan_utils.h"

#include "event.h"

namespace TWE
{
    class window
    {
    public:
        class event_handler
        {
        public:
            event TWE_API pop_event( ) noexcept;
            
            void TWE_API push_event( event event ) noexcept;
            
            bool TWE_API is_keyboard_key_pressed( keyboard::key key_code ) noexcept;
            bool TWE_API is_mouse_button_pressed( mouse::button button_code ) noexcept;
            
            bool TWE_API is_empty( ) const noexcept;

        private:
            static constexpr uint16_t BUFFER_SIZE = 64;
    
            std::bitset<static_cast<size_t>( keyboard::key::last )> key_states_;
            std::bitset<static_cast<size_t>( mouse::button::last )> button_states_;
            
            event buffer_[BUFFER_SIZE];
            uint32_t head_ = 0;
            uint32_t tail_ = 0;
        };

    public:
        explicit TWE_API window( const std::string& title );
        window( const window& rhs ) noexcept = delete;
        TWE_API window( window&& rhs ) noexcept;
        TWE_API ~window( );

        window& operator=( const window& rhs ) noexcept = delete;
        TWE_API window& operator=( window&& rhs ) noexcept;
        
        event TWE_API pop_event( ) noexcept;
        
        bool TWE_API is_event_queue_empty( );
        bool TWE_API is_kbd_key_pressed( keyboard::key key_code ) noexcept;
        bool TWE_API is_mb_pressed( mouse::button button_code ) noexcept;
        
        void TWE_API poll_events( );
        void TWE_API handle_event( const event& event ) noexcept;

        void TWE_API set_title( const std::string& title ) noexcept;
        
        const TWE_API std::string& get_title( ) const noexcept;

        bool TWE_API is_open( ) const noexcept;
        
        vk_return_type<VkSurfaceKHR> TWE_API create_surface( const VkInstance& instance ) const noexcept;
        
        uint32_t TWE_API get_width( ) const noexcept;
        uint32_t TWE_API get_height( ) const noexcept;

    private:
        std::string title_;
        
        bool open_ = false;

#if defined( VK_USE_PLATFORM_WIN32_KHR )
        HINSTANCE win32_instance_ = nullptr;
        HWND win32_window_ = nullptr;
        
        
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )

#elif defined( VK_USE_PLATFORM_XCB_KHR )
        std::unique_ptr<xcb_connection_t, std::function<void( xcb_connection_t* )>> p_xcb_connection_;
        xcb_screen_t* p_xcb_screen_;
        xcb_window_t xcb_window_;
        
        std::unique_ptr<xcb_intern_atom_reply_t> p_xcb_wm_delete_window_;
#endif
        event_handler event_handler_;
 
        struct settings
        {
            uint32_t x_ = 0;
            uint32_t y_ = 0;
            
            uint32_t width_ = 1080;
            uint32_t height_ = 720;
            
            int default_screen_id_ = 0;
            
            bool fullscreen_ = false;
        } settings_;
    };
}

#endif //VULKAN_PROJECT_WINDOW_H
