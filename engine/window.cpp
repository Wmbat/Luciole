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

#include <iostream>

#include "window.h"
#include "console.h"

namespace engine
{
#if defined( VK_USE_PLATFORM_XCB_KHR )
    static inline xcb_intern_atom_reply_t* intern_atom_helper( xcb_connection_t *p_connection, bool only_if_exists, const char *str )
    {
        xcb_intern_atom_cookie_t cookie = xcb_intern_atom( p_connection, only_if_exists, strlen( str ), str);

        return xcb_intern_atom_reply( p_connection, cookie, NULL);
    }
#endif

    window::window( const std::string &title )
            :
            title_( title ),
            open_( true )
    {
#if defined( _WIN32 )

#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )

#elif defined( VK_USE_PLATFORM_XCB_KHR )
        console::log( "Using XCB for Window creation.\n" );

        p_xcb_connection_ = xcb_connect( nullptr, &settings_.default_screen_id_ );

        if( xcb_connection_has_error( p_xcb_connection_ ) )
        {
            console::log(
                    "Failed to connect to the X server.\nDisconnecting from X Server.\nExiting Application.",
                    console::message_priority::error );

            xcb_disconnect( p_xcb_connection_ );
        }
        else
        {
            console::log( "Connection to X Server established.\n" );
        }

        auto monitor_nbr = xcb_setup_roots_iterator( xcb_get_setup ( p_xcb_connection_ ) ).rem;

        /* Get Default monitor */
        auto iter = xcb_setup_roots_iterator( xcb_get_setup( p_xcb_connection_ ) );
        while( monitor_nbr-- > 1 )
        {
            xcb_screen_next( &iter );
        }

        p_xcb_screen_ = iter.data;


        xcb_window_ = xcb_generate_id( p_xcb_connection_ );

        console::log( "XCB window ID generated: " + std::to_string( xcb_window_ ) + '\n' );

        uint32_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        uint32_t value_list[32];
        value_list[0] = p_xcb_screen_->black_pixel;
        value_list[1] =
                XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;

        if( settings_.fullscreen_ )
        {
            console::log( "XCB window fullscreen mode.\n" );

            settings_.width_ = p_xcb_screen_->width_in_pixels;
            settings_.height_ = p_xcb_screen_->height_in_pixels;
        }

        xcb_create_window(
                p_xcb_connection_,                                /* Connection             */
                XCB_COPY_FROM_PARENT,                             /* Depth ( same as root ) */
                xcb_window_,                                      /* Window ID              */
                p_xcb_screen_->root,                              /* Parent Window          */
                settings_.x_position, settings_.y_position,       /* Window Position        */
                settings_.width_, settings_.height_, 10,          /* Window + border Size   */
                XCB_WINDOW_CLASS_INPUT_OUTPUT,                    /* Class                  */
                p_xcb_screen_->root_visual,                       /* Visual                 */
                value_mask, value_list );                         /* Masks                  */

        console::log( "XCB window created.\n" );


        xcb_intern_atom_reply_t* reply = intern_atom_helper( p_xcb_connection_, true, "WM_PROTOCOLS" );
        p_xcb_wm_delete_window_ = intern_atom_helper( p_xcb_connection_, false, "WM_DELETE_WINDOW" );

        xcb_change_property(
                p_xcb_connection_, XCB_PROP_MODE_REPLACE,
                xcb_window_, reply->atom, 4, 32, 1,
                &p_xcb_wm_delete_window_ ->atom );

        xcb_change_property(
                p_xcb_connection_, XCB_PROP_MODE_REPLACE,
                xcb_window_, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                title_.size(), title_.c_str());

        free( reply );

        if ( settings_.fullscreen_ )
        {
            xcb_intern_atom_reply_t *atom_wm_state = intern_atom_helper( p_xcb_connection_, false, "_NET_WM_STATE" );
            xcb_intern_atom_reply_t *atom_wm_fullscreen = intern_atom_helper( p_xcb_connection_, false, "_NET_WM_STATE_FULLSCREEN");
            xcb_change_property(
                    p_xcb_connection_, XCB_PROP_MODE_REPLACE,
                    xcb_window_, atom_wm_state->atom,
                    XCB_ATOM_ATOM, 32, 1,
                    &(atom_wm_fullscreen->atom));

            free( atom_wm_fullscreen );
            free( atom_wm_state );
        }

        xcb_map_window( p_xcb_connection_, xcb_window_ );
        xcb_flush( p_xcb_connection_ );

        console::flush();
#endif
    }
    window::window( window&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    window::~window( )
    {
        free( p_xcb_wm_delete_window_ );

        xcb_flush( p_xcb_connection_ );


        xcb_destroy_window( p_xcb_connection_, xcb_window_ );

        console::log( "XCB window destroyed.\n" );

        xcb_disconnect( p_xcb_connection_ );

        console::log( "Disconnected from X server.\n" );
        console::flush( );
    }

    void window::poll_events( )
    {
#if defined( _WIN32 )

#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )

#elif defined( VK_USE_PLATFORM_XCB_KHR )

        xcb_generic_event_t *event;
        while( ( event = xcb_poll_for_event( p_xcb_connection_ ) ) )
        {
            switch( event->response_type & 0x7f )
            {
                case XCB_CLIENT_MESSAGE:
                {
                    const auto* message_event = reinterpret_cast<const xcb_client_message_event_t *>( event );

                    if ( message_event->data.data32[0] == p_xcb_wm_delete_window_->atom )
                    {
                        open_ = false;
                    }
                }
                break;

                case XCB_KEY_PRESS:
                {
                    const auto* key_press_event = reinterpret_cast<const xcb_key_press_event_t*>( event );

                    keyboard::key_event key_event;
                    key_event.type_ = keyboard::event_type::pressed;
                    key_event.id_ = key_press_event->detail;

                    keyboard_.emplace_event( key_event );
                }
                break;

                case XCB_KEY_RELEASE:
                {
                    const auto* key_release_event = reinterpret_cast<const xcb_key_release_event_t*>( event );

                    keyboard::key_event key_event;
                    key_event.type_ = keyboard::event_type::released;
                    key_event.id_ = key_release_event->detail;

                    keyboard_.emplace_event( key_event );
                }
                break;
            }

            free( event );
        }

#endif
    }

    void window::set_title( const std::string &title )
    {
        title_ = title;

        xcb_change_property(
                p_xcb_connection_, XCB_PROP_MODE_REPLACE,
                xcb_window_, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                title_.size(), title_.c_str() );
    }

    bool window::is_open( ) const noexcept
    {
        return open_;
    }

    vk_return_obj<VkSurfaceKHR> window::create_surface( const VkInstance &instance )
    {
#if defined( _WIN32 )
        const VkWin32SurfaceCreateInfoKHR create_info
        {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        };

#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
        const VkWaylandSurfaceCreateInfoKHR create_info
        {
            .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        };

#elif defined( VK_USE_PLATFORM_XCB_KHR )
        const VkXcbSurfaceCreateInfoKHR create_info
        {
            .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
        };
#endif
    }

    window &window::operator=( window &&rhs ) noexcept
    {
        if( this != &rhs )
        {
            title_ = rhs.title_;
            rhs.title_ = { };

            open_ = rhs.open_;
            rhs.open_ = false;
#if defined( _WIN32 )

#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )

#elif defined( VK_USE_PLATFORM_XCB_KHR )
            p_xcb_connection_ = rhs.p_xcb_connection_;
            rhs.p_xcb_connection_ = nullptr;

            p_xcb_screen_ = rhs.p_xcb_screen_;
            rhs.p_xcb_screen_ = nullptr;

            p_xcb_wm_delete_window_ = rhs.p_xcb_wm_delete_window_;
            rhs.p_xcb_wm_delete_window_ = nullptr;

            xcb_window_ = rhs.xcb_window_;
            rhs.xcb_window_ = 0;
#endif

            window_event_handler_ = rhs.window_event_handler_;
            rhs.window_event_handler_ = { };

            keyboard_ = rhs.keyboard_;
            rhs.keyboard_ = { };

            mouse_ = rhs.mouse_;
            rhs.mouse_ = { };

            settings_ = rhs.settings_;
            rhs.settings_ = { };
        }

        return *this;
    }
}