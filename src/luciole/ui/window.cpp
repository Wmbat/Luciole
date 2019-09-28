/*
 *  Copyright (C) 2018-2019 Wmbat
 *
 *  wmbat@protonmail.com
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

#include <luciole/ui/window.hpp>
#include <luciole/utilities/log.hpp>

#include <algorithm>

namespace ui
{
    
#if defined( VK_USE_PLATFORM_XCB_KHR )
inline window::xcb_intern_atom_uptr intern_atom_helper( xcb_connection_t *p_connection, bool only_if_exists, const std::string& str )
{
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom( p_connection, only_if_exists, static_cast<uint16_t>( str.size() ), str.c_str() );
    
    return window::xcb_intern_atom_uptr( 
        xcb_intern_atom_reply( p_connection, cookie, NULL ),
        []( xcb_intern_atom_reply_t* ptr ){ free( ptr ); } 
    );
}
#endif

window::window( window::create_info_cref_t create_info )
    :
    title_( create_info.value_.title ),
    position_( create_info.value_.position ),
    size_( create_info.value_.size ),
    is_open_( true ),
    is_fullscreen_( false )
{
#if defined( VK_USE_PLATFORM_XCB_KHR )
    core_info ( "Using XCB for window creation." );
    
    /** Connect to X11 window system. */
    p_xcb_connection_ = xcb_connection_uptr(
        xcb_connect( nullptr, &default_screen_id_ ),
        []( xcb_connection_t* p ) { xcb_disconnect( p ); } );

    if( xcb_connection_has_error( p_xcb_connection_.get() ) )
    {
        core_error( "Failed to connecte to the X server.\nDisconnecting from X server.\nExiting Application." );
    
        p_xcb_connection_.reset( );
    }
    else
    {
        core_info( "Connection to X server established." );
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
    core_info( "Window ID generated: " + std::to_string( xcb_window_ ) + '.' );

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
    
    if( is_fullscreen_ )
    {
        core_info( "Window fullscreen mode." );
    
        size_.x = p_xcb_screen_->width_in_pixels;
        size_.y = p_xcb_screen_->height_in_pixels;
    }

    xcb_create_window(
        p_xcb_connection_.get(),                          /* Connection             */
        XCB_COPY_FROM_PARENT,                             /* Depth ( same as root ) */
        xcb_window_,                                      /* Window ID              */
        p_xcb_screen_->root,                              /* Parent Window          */
        position_.x, position_.y,                       /* Window Position        */
        size_.x, size_.y, 10,          /* Window + border Size   */
        XCB_WINDOW_CLASS_INPUT_OUTPUT,                    /* Class                  */
        p_xcb_screen_->root_visual,                       /* Visual                 */
        value_mask, value_list                            /* Masks                  */
    );

    core_info( "Window created." );

    auto p_reply = intern_atom_helper( p_xcb_connection_.get(), true, "WM_PROTOCOLS" );

    p_xcb_wm_delete_window_ = intern_atom_helper( p_xcb_connection_.get(), false, "WM_DELETE_WINDOW" );

    /** Allows checking of window closing event. */
    xcb_change_property(
        p_xcb_connection_.get(), XCB_PROP_MODE_REPLACE,
        xcb_window_, p_reply->atom, 4, 32, 1,
        &p_xcb_wm_delete_window_->atom 
    );

    /** Change the title of the window. */
    xcb_change_property(
        p_xcb_connection_.get(), XCB_PROP_MODE_REPLACE,
        xcb_window_, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
        title_.size(), title_.c_str() 
    );

    /** Set the window to fullscreen if fullscreen is enabled. */
    if ( is_fullscreen_ )
    {
        auto p_atom_wm_state = intern_atom_helper( p_xcb_connection_.get(), false, "_NET_WM_STATE" );
        auto p_atom_wm_fullscreen = intern_atom_helper( p_xcb_connection_.get(), false, "_NET_WM_STATE_FULLSCREEN");
    
        xcb_change_property(
            p_xcb_connection_.get(), XCB_PROP_MODE_REPLACE,
            xcb_window_, p_atom_wm_state->atom,
            XCB_ATOM_ATOM, 32, 1,
            &p_atom_wm_fullscreen->atom 
        );
    }
    xcb_map_window( p_xcb_connection_.get(), xcb_window_ );
    xcb_flush( p_xcb_connection_.get() );
#endif
}
window::window( window&& wnd )
{
    *this = std::move( wnd );
}
window::~window()
{

}

window& window::operator=( window&& rhs )
{
    if ( this != &rhs )
    {
        title_ = std::move( rhs.title_ );
        
        position_ = rhs.position_;
        rhs.position_ = {};

        size_ = rhs.size_;
        rhs.size_ = {};

        is_open_ = rhs.is_open_;
        rhs.is_open_ = false;

        is_fullscreen_ = rhs.is_fullscreen_;
        rhs.is_fullscreen_ = false;

#if defined( VK_USE_PLATFORM_XCB_KHR )
        p_xcb_connection_ = std::move( rhs.p_xcb_connection_ );
        p_xcb_screen_ = std::move( p_xcb_screen_ );
        xcb_window_ = std::move( xcb_window_ );
        p_xcb_wm_delete_window_ = std::move( rhs.p_xcb_wm_delete_window_ );

        default_screen_id_ = rhs.default_screen_id_;
        rhs.default_screen_id_ = -1;
#endif
    }

    return *this;
}

bool window::is_open( )
{
    return is_open_;
}

void window::poll_events( )
{
#if defined( VK_USE_PLATFORM_XCB_KHR )
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
                    is_open_ = false;
                    
                    const auto event = window_close_event( )
                        .set_is_closed( true );
                    
                    window_close_event_.send_message( event );
                }
            } break;
            case XCB_DESTROY_NOTIFY:
            {
                is_open_ = false;

                const auto event = window_close_event( )
                    .set_is_closed( true );

                window_close_event_.send_message( event );
            } break;
            case XCB_CONFIGURE_NOTIFY:
            {
                const auto *motion_event = reinterpret_cast<const xcb_configure_notify_event_t *>( e );

                position_.x = static_cast<uint32_t>( motion_event->x );
                position_.y = static_cast<uint32_t>( motion_event->y );

                if ( size_.x != static_cast<uint32_t>( motion_event->width ) &&
                     size_.y != static_cast<uint32_t>( motion_event->height ))
                {
                    size_.x = static_cast<uint32_t>( motion_event->width );
                    size_.y = static_cast<uint32_t>( motion_event->height );
    
                    const auto event = framebuffer_resize_event( )
                        .set_size( { size_.x, size_.y } );
    
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
#endif
}

std::variant<VkSurfaceKHR, vk::error::type> 
window::create_surface( vk::instance_t instance ) const
{
#if defined( VK_USE_PLATFORM_XCB_KHR )
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    VkXcbSurfaceCreateInfoKHR create_info{ };
    create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    create_info.connection = p_xcb_connection_.get( );
    create_info.window = xcb_window_;

    switch ( vkCreateXcbSurfaceKHR( instance.value_, &create_info, nullptr, &surface ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
        default:
            return surface;
    }
#endif
}

glm::uvec2 window::get_size() const
{
    return size_;
}

} // Namespace ui