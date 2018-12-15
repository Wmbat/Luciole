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

#include "../event.h"
#include "../TWE_core.h"
#include "../vulkan_utils.h"

namespace TWE
{
    class base_window
    {
    public:
        virtual ~base_window( ) = default;
        
        virtual void TWE_API poll_events( ) = 0;
    
        bool TWE_API is_open( ) const noexcept;
    
        virtual vk::ResultValue<vk::SurfaceKHR> TWE_API create_surface( const vk::Instance& instance ) const noexcept = 0;
    
        uint32_t TWE_API get_width( ) const noexcept;
        uint32_t TWE_API get_height( ) const noexcept;
        
        void TWE_API add_key_listener( const key_event_delg& delg );
        void TWE_API add_mouse_button_listener( const mouse_button_event_delg& delg );
        void TWE_API add_mouse_motion_listener( const mouse_motion_event_delg& delg );
        void TWE_API add_window_close_listener( const window_close_event_delg& delg );
        void TWE_API add_framebuffer_resize_listener( const framebuffer_resize_event_delg& delg );
        
    protected:
        std::string title_;
    
        bool open_ = false;
        
        //event_handler event_handler_;
    
        struct settings
        {
            uint32_t x_ = 100;
            uint32_t y_ = 100;
        
            uint32_t width_ = 1080;
            uint32_t height_ = 720;
        
            int default_screen_id_ = 0;
        
            bool fullscreen_ = false;
        } settings_;
        
        event<const key_event> key_event_;
        event<const mouse_button_event> mouse_button_event_;
        event<const mouse_motion_event> mouse_motion_event_;
        event<const window_close_event> window_close_event_;
        event<const framebuffer_resize_event> frame_buffer_resize_event_;
    };
}

#endif //BASE_WINDOW_H