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

#include "../TWE/window.h"
#include "../TWE/console.h"

namespace TWE
{
#if defined( VK_USE_PLATFORM_XCB_KHR )
    static inline std::unique_ptr<xcb_intern_atom_reply_t> intern_atom_helper( xcb_connection_t *p_connection, bool only_if_exists, const std::string& str )
    {
        xcb_intern_atom_cookie_t cookie = xcb_intern_atom( p_connection, only_if_exists, str.size(), str.c_str() );

        return std::unique_ptr<xcb_intern_atom_reply_t>( xcb_intern_atom_reply( p_connection, cookie, NULL ) );
    }
#endif

    window::event_handler::event_handler( )
        :
        num_elem_( 0 ),
        head_( 0 ),
        tail_( 0 )
    { }

    window::event_handler::event window::event_handler::pop_event( ) noexcept
    {
        auto ret = buffer_[head_];

        buffer_[head_] = { };

        --num_elem_;
        head_ = ( head_ + 1 ) % BUFFER_SIZE;

        return ret;
    }

    void window::event_handler::emplace_event( window::event_handler::event event ) noexcept
    {
        if( num_elem_ == BUFFER_SIZE )
        {
            pop_event();
        }

        buffer_[tail_] = event;

        ++num_elem_;
        tail_ = ( tail_ + 1 ) % BUFFER_SIZE;
    }

    bool window::event_handler::is_empty( ) const noexcept
    {
        return num_elem_ == 0;
    }


    window::window( const std::string &title )
            :
            title_( title ),
            open_( true )
    {
#if defined( _WIN32 )

#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )

#elif defined( VK_USE_PLATFORM_XCB_KHR )
        console::log( "Using XCB for Window creation.\n" );

        /** Connect to X11 window system. */
        p_xcb_connection_ = std::unique_ptr<xcb_connection_t, std::function<void( xcb_connection_t* )>>(
                xcb_connect( nullptr, &settings_.default_screen_id_ ),
                []( xcb_connection_t* p ) { xcb_disconnect( p ); } );

        if( xcb_connection_has_error( p_xcb_connection_.get() ) )
        {
            console::log(
                    "Failed to connect to the X server.\nDisconnecting from X Server.\nExiting Application.",
                    console::message_priority::error );

            p_xcb_connection_.reset( );
        }
        else
        {
            console::log( "Connection to X Server established.\n" );
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
        console::log( "XCB window ID generated: " + std::to_string( xcb_window_ ) + '\n' );


        uint32_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        uint32_t value_list[32];
        value_list[0] = p_xcb_screen_->black_pixel;
        value_list[1] =
                XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                XCB_EVENT_MASK_FOCUS_CHANGE |
                XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                XCB_EVENT_MASK_POINTER_MOTION;

        if( settings_.fullscreen_ )
        {
            console::log( "XCB window fullscreen mode.\n" );

            settings_.width_ = p_xcb_screen_->width_in_pixels;
            settings_.height_ = p_xcb_screen_->height_in_pixels;
        }

        xcb_create_window(
                p_xcb_connection_.get(),                          /* Connection             */
                XCB_COPY_FROM_PARENT,                             /* Depth ( same as root ) */
                xcb_window_,                                      /* Window ID              */
                p_xcb_screen_->root,                              /* Parent Window          */
                settings_.x_position, settings_.y_position,       /* Window Position        */
                settings_.width_, settings_.height_, 10,          /* Window + border Size   */
                XCB_WINDOW_CLASS_INPUT_OUTPUT,                    /* Class                  */
                p_xcb_screen_->root_visual,                       /* Visual                 */
                value_mask, value_list );                         /* Masks                  */

        console::log( "XCB window created.\n" );


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

        console::flush();
#endif
    }
    window::window( window&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    window::~window( )
    {
        xcb_destroy_window( p_xcb_connection_.get(), xcb_window_ );

        console::log( "XCB window destroyed.\n" );

        console::log( "Disconnected from X server.\n" );
        console::flush( );
    }

    void window::poll_events( )
    {
#if defined( _WIN32 )

#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )

#elif defined( VK_USE_PLATFORM_XCB_KHR )

        xcb_generic_event_t *event;
        while( ( event = xcb_poll_for_event( p_xcb_connection_.get() ) ) )
        {
            switch( event->response_type & 0x7f )
            {
                case XCB_CLIENT_MESSAGE:
                {
                    const auto* message_event = reinterpret_cast<const xcb_client_message_event_t*>( event );

                    if ( message_event->data.data32[0] == p_xcb_wm_delete_window_->atom )
                    {
                        open_ = false;
                    }
                }
                break;
                case XCB_DESTROY_NOTIFY:
                {
                    open_ = false;
                }
                break;

                case XCB_CONFIGURE_NOTIFY:
                {
                    const auto* motion_event = reinterpret_cast<const xcb_configure_notify_event_t*>( event );

                    const event_handler::event window_event
                    {
                        .type_ = event_handler::type::window_resize,
                        .x_ = static_cast<uint32_t>( motion_event->width ),
                        .y_ = static_cast<uint32_t>( motion_event->height )
                    };

                    event_handler_.emplace_event( window_event );

                    const event_handler::event window_move_event
                    {
                        .type_ = event_handler::type::window_move,
                        .x_ = static_cast<uint32_t>( motion_event->x ),
                        .y_ = static_cast<uint32_t>( motion_event->y )
                    };

                    event_handler_.emplace_event( window_move_event );
                }
                case XCB_FOCUS_IN:
                {
                    const auto* focus_in_event = reinterpret_cast<const xcb_focus_in_event_t*>( event );

                    const event_handler::event window_event
                    {
                        .type_ = event_handler::type::window_focus_in
                    };

                    event_handler_.emplace_event( window_event );
                }
                break;
                case XCB_FOCUS_OUT:
                {
                    const auto* focus_out_event = reinterpret_cast<const xcb_focus_out_event_t*>( event );

                    const event_handler::event window_event
                    {
                        .type_ = event_handler::type ::window_focus_out
                    };

                    event_handler_.emplace_event( window_event );
                }
                break;

                case XCB_KEY_PRESS:
                {
                    const auto* key_press_event = reinterpret_cast<const xcb_key_press_event_t*>( event );

                    const keyboard::key_event key_event
                    {
                        .id_ = key_press_event->detail,
                        .type_ = keyboard::event_type::pressed
                    };

                    keyboard_.emplace_event( key_event );
                }
                break;
                case XCB_KEY_RELEASE:
                {
                    const auto* key_release_event = reinterpret_cast<const xcb_key_release_event_t*>( event );

                    const keyboard::key_event key_event
                    {
                        .id_ = key_release_event->detail,
                        .type_ = keyboard::event_type ::released
                    };

                    keyboard_.emplace_event( key_event );
                }
                break;

                case XCB_BUTTON_PRESS:
                {
                    const auto* button_press_event = reinterpret_cast<const xcb_button_press_event_t*>( event );

                    const mouse::button_event button_event
                    {
                        .button_ = static_cast<mouse::button>( button_press_event->detail ),
                        .type_ = mouse::type::pressed
                    };

                    mouse_.update_pos( button_press_event->event_x, button_press_event->event_y );
                    mouse_.emplace_button_event( button_event );
                }
                break;
                case XCB_BUTTON_RELEASE:
                {
                    const auto* button_release_event = reinterpret_cast<const xcb_button_release_event_t*>( event );

                    const mouse::button_event button_event
                    {
                        .button_ = static_cast<mouse::button>( button_release_event->detail ),
                        .type_ = mouse::type::released
                    };

                    mouse_.update_pos( button_release_event->event_x, button_release_event->event_y );
                    mouse_.emplace_button_event( button_event );
                }
                break;

                case XCB_MOTION_NOTIFY:
                {
                    const auto* cursor_motion = reinterpret_cast<const xcb_motion_notify_event_t*>( event );

                    mouse_.update_pos( cursor_motion->event_x, cursor_motion->event_y );
                }
                break;
            }

            free( event );
        }
#endif
    }

    void window::set_title( const std::string &title ) noexcept
    {
        xcb_change_property(
                p_xcb_connection_.get(), XCB_PROP_MODE_REPLACE,
                xcb_window_, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                title.size(), title.c_str() );

        xcb_flush( p_xcb_connection_.get() );
    }

    const std::string& window::get_title( ) const noexcept
    {
        return title_;
    }

    bool window::is_open( ) const noexcept
    {
        return open_;
    }

    vk_return_type<VkSurfaceKHR> window::create_surface( const VkInstance &instance ) const noexcept
    {
        VkSurfaceKHR surface;

#if defined( _WIN32 )
        const VkWin32SurfaceCreateInfoKHR create_info
        {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr;
            .flags = { },
        };

#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
        const VkWaylandSurfaceCreateInfoKHR create_info
        {
            .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = { },
        };

#elif defined( VK_USE_PLATFORM_XCB_KHR )
        const VkXcbSurfaceCreateInfoKHR create_info
        {
            .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = { },
            .connection = p_xcb_connection_.get(),
            .window = xcb_window_
        };

        return { vkCreateXcbSurfaceKHR( instance, &create_info, nullptr, &surface ), surface };
#endif
    }

///////////////////////////////// Window Event //////////////////////////////////

    bool window::no_window_event( ) const noexcept
    {
        return event_handler_.is_empty();
    }

    window::event_handler::event window::pop_window_event( ) noexcept
    {
        return event_handler_.pop_event();
    }

    void window::handle_event( const window::event_handler::event &event ) noexcept
    {
        if( event.type_ == event_handler::type::window_move )
        {
            settings_.x_position = event.x_;
            settings_.y_position = event.y_;
        }

        if( event.type_ == event_handler::type::window_resize )
        {
            settings_.width_ = event.x_;
            settings_.height_ = event.y_;
        }
    }

/////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// Mouse Event //////////////////////////////////

    bool window::no_button_event( ) const noexcept
    {
        return mouse_.is_button_empty();
    }

    bool window::is_button_pressed( mouse::button button ) const noexcept
    {
        return mouse_.is_button_pressed( button );
    }


    glm::i32vec2 window::cursor_position( ) const noexcept
    {
        return mouse_.cursor_pos();
    }

    mouse::button_event window::pop_button_event( ) noexcept
    {
        return mouse_.pop_button_event();
    }

/////////////////////////////////////////////////////////////////////////////////

//////////////////////////////// Keyboard Event /////////////////////////////////

    bool window::no_key_event( )
    {
        return keyboard_.empty();
    }
    bool window::is_key_pressed( keyboard::key key_code ) const noexcept
    {
        return keyboard_.is_key_pressed( key_code );
    }

    keyboard::key_event window::pop_key_event( )
    {
        return keyboard_.pop_key_event();
    }

/////////////////////////////////////////////////////////////////////////////////



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
            p_xcb_connection_ = std::move( p_xcb_connection_ );

            p_xcb_screen_ = rhs.p_xcb_screen_;
            rhs.p_xcb_screen_ = nullptr;

            p_xcb_wm_delete_window_ = std::move( rhs.p_xcb_wm_delete_window_ );

            xcb_window_ = rhs.xcb_window_;
            rhs.xcb_window_ = 0;
#endif

            event_handler_ = rhs.event_handler_;
            rhs.event_handler_ = { };

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