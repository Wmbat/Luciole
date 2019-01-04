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

#include "xcb_window.hpp"
#include "../utilities/log.hpp"

#if defined( VK_USE_PLATFORM_XCB_KHR )

/*
#define explicit c_explicit             // Avoid mixing up "explicit" with the C++11 explicit
#include <xcb/xkb.h>
#undef explicit
*/

namespace twe
{
    static inline std::unique_ptr<xcb_intern_atom_reply_t> intern_atom_helper( xcb_connection_t *p_connection, bool only_if_exists, const std::string& str )
    {
        xcb_intern_atom_cookie_t cookie = xcb_intern_atom( p_connection, only_if_exists, static_cast<uint16_t>( str.size() ), str.c_str() );
        
        return std::unique_ptr<xcb_intern_atom_reply_t>( xcb_intern_atom_reply( p_connection, cookie, NULL ) );
    }
    
    xcb_window::xcb_window( const std::string& title )
    {
        core_info ( "Using XCB for window creation." );
    
        title_ = title;
        open_ = true;
        
        /** Connect to X11 window system. */
        p_xcb_connection_ = std::unique_ptr<xcb_connection_t, std::function<void( xcb_connection_t* )>>(
            xcb_connect( nullptr, &settings_.default_screen_id_ ),
            []( xcb_connection_t* p ) { xcb_disconnect( p ); } );
    
        if( xcb_connection_has_error( p_xcb_connection_.get() ) )
        {
            core_error( "Failed to connecte to the X server.\nDisconnecting from X server.\nExiting Application." );
        
            p_xcb_connection_.reset( );
        }
        else
        {
            core_info( "XCB -> Connection to X server established." );
        }
    
        /** Get Default monitor */
        auto monitor_nbr = xcb_setup_roots_iterator( xcb_get_setup( p_xcb_connection_.get() ) ).rem;
    
        /** Loop through all available monitors. */
        auto iter = xcb_setup_roots_iterator( xcb_get_setup( p_xcb_connection_.get() ) );
        while( monitor_nbr-- > 1 )
        {
            xcb_screen_next( &iter );   // TODO: Allow user to pick their prefered monitor.
        }
        p_xcb_screen_ = iter.data;
    
        xcb_window_ = xcb_generate_id( p_xcb_connection_.get() );
        core_info( "XCB -> window ID generated: " + std::to_string( xcb_window_ ) + '.' );
    
        uint32_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        uint32_t value_list[32];
        value_list[0] = p_xcb_screen_->black_pixel;
        value_list[1] =
            XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
            XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
            XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
            XCB_EVENT_MASK_POINTER_MOTION;
    
        /*
        xcb_xkb_use_extension( p_xcb_connection_.get(), XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION );
        
        xcb_xkb_per_client_flags( p_xcb_connection_.get(), XCB_XKB_ID_USE_CORE_KBD,
                                  XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT,
                                  XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT,
                                  0,0,0 );
                                  */
        
        if( settings_.fullscreen_ )
        {
            core_info( "XCB -> window fullscreen mode." );
        
            settings_.width_ = p_xcb_screen_->width_in_pixels;
            settings_.height_ = p_xcb_screen_->height_in_pixels;
        }
    
        xcb_create_window(
            p_xcb_connection_.get(),                          /* Connection             */
            XCB_COPY_FROM_PARENT,                             /* Depth ( same as root ) */
            xcb_window_,                                      /* Window ID              */
            p_xcb_screen_->root,                              /* Parent Window          */
            settings_.x_, settings_.y_,                       /* Window Position        */
            settings_.width_, settings_.height_, 10,          /* Window + border Size   */
            XCB_WINDOW_CLASS_INPUT_OUTPUT,                    /* Class                  */
            p_xcb_screen_->root_visual,                       /* Visual                 */
            value_mask, value_list );                         /* Masks                  */
    
        core_info( "XCB -> window created." );
    
        auto reply = intern_atom_helper( p_xcb_connection_.get(), true, "WM_PROTOCOLS" );
    
        p_xcb_wm_delete_window_ = intern_atom_helper( p_xcb_connection_.get(), false, "WM_DELETE_WINDOW" );
    
        /** Allows checking of window closing event. */
        xcb_change_property(
            p_xcb_connection_.get(), XCB_PROP_MODE_REPLACE,
            xcb_window_, reply->atom, 4, 32, 1,
            &p_xcb_wm_delete_window_->atom );
    
        /** Change the title of the window. */
        xcb_change_property(
            p_xcb_connection_.get(), XCB_PROP_MODE_REPLACE,
            xcb_window_, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
            title_.size(), title_.c_str() );
    
    
        /** Set the window to fullscreen if fullscreen is enabled. */
        if ( settings_.fullscreen_ )
        {
            auto p_atom_wm_state = intern_atom_helper( p_xcb_connection_.get(), false, "_NET_WM_STATE" );
            auto p_atom_wm_fullscreen = intern_atom_helper( p_xcb_connection_.get(), false, "_NET_WM_STATE_FULLSCREEN");
        
            xcb_change_property(
                p_xcb_connection_.get(), XCB_PROP_MODE_REPLACE,
                xcb_window_, p_atom_wm_state->atom,
                XCB_ATOM_ATOM, 32, 1,
                &p_atom_wm_fullscreen->atom );
        }
    
    
        xcb_map_window( p_xcb_connection_.get(), xcb_window_ );
        xcb_flush( p_xcb_connection_.get() );
    }
    xcb_window::xcb_window( xcb_window&& rhs ) noexcept
    {
        *this = std::move ( rhs );
    }
    xcb_window::~xcb_window( )
    {
        xcb_destroy_window( p_xcb_connection_.get(), xcb_window_ );
    
        core_info( "XCB -> window destroyed." );
        core_info( "XCB -> Disconnected from X server." );
    }
    
    xcb_window& xcb_window::operator=( xcb_window &&rhs ) noexcept
    {
        if( this != &rhs )
        {
            title_ = rhs.title_;
            rhs.title_ = { };
        
            open_ = rhs.open_;
            rhs.open_ = false;
        
            p_xcb_connection_ = std::move( p_xcb_connection_ );
        
            p_xcb_screen_ = rhs.p_xcb_screen_;
            rhs.p_xcb_screen_ = nullptr;
        
            p_xcb_wm_delete_window_ = std::move( rhs.p_xcb_wm_delete_window_ );
        
            xcb_window_ = rhs.xcb_window_;
            rhs.xcb_window_ = 0;
            
            settings_ = rhs.settings_;
            rhs.settings_ = { };
        }
    
        return *this;
    }
    
    void xcb_window::poll_events( )
    {
        xcb_generic_event_t *e;
        while( ( e = xcb_poll_for_event( p_xcb_connection_.get() ) ) )
        {
            switch ( e->response_type & 0x7f )
            {
                case XCB_CLIENT_MESSAGE:
                {
                    const auto *message_event = reinterpret_cast<const xcb_client_message_event_t *>( e );
                
                    if ( message_event->data.data32[0] == p_xcb_wm_delete_window_->atom )
                    {
                        open_ = false;
                        
                        const auto event = window_close_event( )
                            .set_is_closed( true );
                        
                        window_close_event_.send_message( event );
                    }
                } break;
                case XCB_DESTROY_NOTIFY:
                {
                    open_ = false;
    
                    const auto event = window_close_event( )
                        .set_is_closed( true );
    
                    window_close_event_.send_message( event );
                } break;
                case XCB_CONFIGURE_NOTIFY:
                {
                    const auto *motion_event = reinterpret_cast<const xcb_configure_notify_event_t *>( e );
    
                    settings_.x_ = static_cast<uint32_t>( motion_event->x );
                    settings_.y_ = static_cast<uint32_t>( motion_event->y );
    
                    if ( settings_.width_ != static_cast<uint32_t>( motion_event->width ) &&
                         settings_.height_ != static_cast<uint32_t>( motion_event->height ))
                    {
                        settings_.width_ = static_cast<uint32_t>( motion_event->width );
                        settings_.height_ = static_cast<uint32_t>( motion_event->height );
        
                        const auto event = framebuffer_resize_event( )
                            .set_size( { settings_.width_, settings_.height_ } );
        
                        framebuffer_resize_event_.send_message( event );
                    }
                } break;
                case XCB_KEY_PRESS:
                {
                    const auto *xcb_key_press = reinterpret_cast<const xcb_key_press_event_t *>( e );
                
                    const auto event = key_event( )
                        .set_code( static_cast<keyboard::key>( xcb_key_press->detail ) )
                        .set_state( keyboard::key_state::pressed );
                    
                    key_event_.send_message( event );
                } break;
                case XCB_KEY_RELEASE:
                {
                    const auto *xcb_key_release = reinterpret_cast<const xcb_key_release_event_t *>( e );
                
                    const auto event = key_event( )
                        .set_code( static_cast<keyboard::key>( xcb_key_release->detail ) )
                        .set_state( keyboard::key_state::released );
                
                    key_event_.send_message( event );
                } break;
                case XCB_BUTTON_PRESS:
                {
                    const auto *button_press_event = reinterpret_cast<const xcb_button_press_event_t *>( e );
    
                    const auto event = mouse_button_event( )
                        .set_code( static_cast<mouse::button>( button_press_event->detail ))
                        .set_state( mouse::button_state::pressed )
                        .set_position( {
                                           static_cast<int32_t>( button_press_event->event_x ),
                                           static_cast<int32_t>( button_press_event->event_y )
                                       } );
                    
                    mouse_button_event_.send_message( event );
                } break;
                case XCB_BUTTON_RELEASE:
                {
                    const auto *button_release_event = reinterpret_cast<const xcb_button_release_event_t *>( e );
    
                    const auto event = mouse_button_event( )
                        .set_code( static_cast<mouse::button>( button_release_event->detail ))
                        .set_state( mouse::button_state::pressed )
                        .set_position( {
                                           static_cast<int32_t>( button_release_event->event_x ),
                                           static_cast<int32_t>( button_release_event->event_y )
                                       } );
    
                    mouse_button_event_.send_message( event );
                } break;
                case XCB_MOTION_NOTIFY:
                {
                    const auto *cursor_motion = reinterpret_cast<const xcb_motion_notify_event_t *>( e );
    
                    const auto event = mouse_motion_event( )
                        .set_position( {
                                           static_cast<int32_t>( cursor_motion->event_x ),
                                           static_cast<int32_t>( cursor_motion->event_y )
                                       } );
    
                    mouse_motion_event_.send_message( event );
                } break;
            }
        
            free( e );
        }
    }
    
    vk::UniqueSurfaceKHR xcb_window::create_surface( const vk::Instance& instance ) const noexcept
    {
        const auto create_info = vk::XcbSurfaceCreateInfoKHR( )
            .setConnection( p_xcb_connection_.get() )
            .setWindow( xcb_window_ );
        
        return instance.createXcbSurfaceKHRUnique( create_info );
    }
}

#endif