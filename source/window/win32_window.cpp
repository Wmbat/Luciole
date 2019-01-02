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

#include "log.h"
#include "window/win32_window.h"

#if defined( VK_USE_PLATFORM_WIN32_KHR )

#include <Windows.h>

namespace twe
{
    win32_window::win32_window ( const std::string& title )
    {
        core_info ( "Using Win32 for window creation." );

        title_ = title;

        h_inst_ = GetModuleHandle ( NULL );

        WNDCLASSEX wc
        {
            sizeof ( WNDCLASSEX ),              // cbSize
            CS_HREDRAW | CS_VREDRAW,            // style
            handle_msg_setup,                   // lpfnWndProc
            0,                                  // cbClsExtra
            0,                                  // cbWndExtra
            h_inst_,                            // hInstance
            nullptr,                            // hIcon
            LoadCursor ( nullptr,IDC_ARROW ),   // hCursor
            nullptr,                            // hbrBackground
            nullptr,                            // lpszMenuName
            wnd_class_name_,                    // lpszClassName
            nullptr                             // hIconSm
        };

        RegisterClassEx ( &wc );

        RECT wnd_rect
        {
            settings_.x_,                       // left
            settings_.y_,                       // top
            settings_.x_ + settings_.width_,    // right
            settings_.y_ + settings_.height_    // bottom
        };

        AdjustWindowRect ( &wnd_rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE );

        h_wnd_ = CreateWindow ( wnd_class_name_, title_.c_str ( ),
                                WS_OVERLAPPEDWINDOW,
                                settings_.x_, settings_.y_,
                                wnd_rect.right - wnd_rect.left,
                                wnd_rect.bottom - wnd_rect.top,
                                nullptr, nullptr, h_inst_, this );
        if( h_wnd_ == nullptr )
        {
            UnregisterClass ( wnd_class_name_, h_inst_ );
        }

        open_ = true;

        ShowWindow ( h_wnd_, SW_SHOWDEFAULT );
        UpdateWindow ( h_wnd_ );
    }
    win32_window::win32_window ( win32_window&& rhs ) noexcept
    {
        *this = std::move ( rhs );
    }
    win32_window::~win32_window ( )
    {
        UnregisterClass ( wnd_class_name_, h_inst_ );
    }

    win32_window& win32_window::operator=( win32_window&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            title_ = rhs.title_;
            rhs.title_ = { };

            open_ = rhs.open_;
            rhs.open_ = false;

            h_inst_ = rhs.h_inst_;
            rhs.h_inst_ = nullptr;

            h_wnd_ = rhs.h_wnd_;
            rhs.h_wnd_ = nullptr;

            settings_ = rhs.settings_;
            rhs.settings_ = { };
        }

        return *this;
    }

    void win32_window::poll_events ( )
    {
        MSG msg = { 0 };

        while( PeekMessage ( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage ( &msg );

            DispatchMessage ( &msg );

            if( msg.message == WM_QUIT )
            {
                open_ = false;
            }
        }
    };

    vk::UniqueSurfaceKHR win32_window::create_surface ( const vk::Instance& instance ) const noexcept
    {
        VkSurfaceKHR surface;

        const VkWin32SurfaceCreateInfoKHR create_info
        {
            VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,            // sType
            nullptr,                                                    // pNext
            { },                                                        // flags
            h_inst_,                                                    // hinstance
            h_wnd_                                                      // hwnd
        };

        return instance.createWin32SurfaceKHRUnique ( create_info );
    }

    LRESULT __stdcall win32_window::handle_msg_setup ( HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param )
    {
        // use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
        if( msg == WM_NCCREATE )
        {
            // extract ptr to window class from creation data
            const CREATESTRUCTW* const p_create = reinterpret_cast< CREATESTRUCTW* >( l_param );
            win32_window* const p_wnd = reinterpret_cast< win32_window* >( p_create->lpCreateParams );

            // set WinAPI-managed user data to store ptr to window class
            SetWindowLongPtr ( h_wnd, GWLP_USERDATA, reinterpret_cast< LONG_PTR >( p_wnd ) );
            // set message proc to normal (non-setup) handler now that setup is finished
            SetWindowLongPtr ( h_wnd, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( &win32_window::handle_msg_thunk ) );
            // forward message to window class handler
            return p_wnd->handle_msg ( h_wnd, msg, w_param, l_param );
        }
        // if we get a message before the WM_NCCREATE message, handle with default handler
        return DefWindowProc ( h_wnd, msg, w_param, l_param );
    }

    LRESULT __stdcall win32_window::handle_msg_thunk ( HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param )
    {
        // retrieve ptr to window class
        win32_window* const p_wnd = reinterpret_cast<win32_window*>( GetWindowLongPtr ( h_wnd, GWLP_USERDATA ) );
        // forward message to window class handler
        return p_wnd->handle_msg ( h_wnd, msg, w_param, l_param );
    }

    LRESULT win32_window::handle_msg ( HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param )
    {
        switch( msg )
        {
            case WM_DESTROY:
            {
                open_ = false;

                const auto event = window_close_event ( )
                    .set_is_closed ( open_ );


                window_close_event_.send_message ( event );

                PostQuitMessage ( 0 );
            } break;
            case WM_KEYDOWN:
            {
                const auto event = key_event ( )
                    .set_code ( static_cast< keyboard::key >( w_param ) )
                    .set_state ( keyboard::key_state::pressed );

                key_event_.send_message ( event );
            } break;
            case WM_KEYUP:
            {
                const auto event = key_event ( )
                    .set_code ( static_cast< keyboard::key >( w_param ) )
                    .set_state ( keyboard::key_state::released );

                key_event_.send_message ( event );
            } break;
            case WM_LBUTTONDOWN:
            {
                const auto points = MAKEPOINTS ( l_param );
                
                if ( static_cast<uint32_t>( points.x ) > 0 && static_cast<uint32_t>( points.x ) < settings_.width_ &&
                     static_cast<uint32_t>( points.y ) > 0 && static_cast<uint32_t>( points.y ) < settings_.height_ )
                {
                    const auto event = mouse_button_event ( )
                        .set_code ( mouse::button::l_button )
                        .set_state ( mouse::button_state::pressed )
                        .set_position ( glm::i32vec2( points.x, points.y ) );

                    mouse_button_event_.send_message ( event );
                }
            } break;
            case WM_LBUTTONUP:
            {
                const auto points = MAKEPOINTS ( l_param );
                if ( points.x > 0 && points.x < settings_.width_ &&
                     points.y > 0 && points.y < settings_.height_ )
                {
                    const auto event = mouse_button_event ( )
                        .set_code ( mouse::button::l_button )
                        .set_state ( mouse::button_state::released )
                        .set_position ( glm::i32vec2 ( points.x, points.y ) );

                    mouse_button_event_.send_message ( event );
                }
            } break;
            case WM_MBUTTONDOWN:
            {
                const auto points = MAKEPOINTS ( l_param );

                if ( points.x > 0 && points.x < settings_.width_ &&
                     points.y > 0 && points.y < settings_.height_ )
                {
                    const auto event = mouse_button_event ( )
                        .set_code ( mouse::button::scroll_button )
                        .set_state ( mouse::button_state::pressed )
                        .set_position ( glm::i32vec2 ( points.x, points.y ) );

                    mouse_button_event_.send_message ( event );
                }
            } break;
            case WM_MBUTTONUP:
            {
                const auto points = MAKEPOINTS ( l_param );

                if ( points.x > 0 && points.x < settings_.width_ &&
                     points.y > 0 && points.y < settings_.height_ )
                {
                    const auto event = mouse_button_event ( )
                        .set_code ( mouse::button::scroll_button )
                        .set_state ( mouse::button_state::released )
                        .set_position ( glm::i32vec2 ( points.x, points.y ) );

                    mouse_button_event_.send_message ( event );
                }
            } break;
            case WM_RBUTTONDOWN:
            {
                const auto points = MAKEPOINTS ( l_param );

                if ( points.x > 0 && points.x < settings_.width_ &&
                     points.y > 0 && points.y < settings_.height_ )
                {
                    const auto event = mouse_button_event ( )
                        .set_code ( mouse::button::r_button )
                        .set_state ( mouse::button_state::pressed )
                        .set_position ( glm::i32vec2 ( points.x, points.y ) );

                    mouse_button_event_.send_message ( event );
                }
            } break;
            case WM_RBUTTONUP:
            {
                const auto points = MAKEPOINTS ( l_param );
                if ( points.x > 0 && points.x < settings_.width_ &&
                     points.y > 0 && points.y < settings_.height_ )
                {
                    const auto event = mouse_button_event ( )
                        .set_code ( mouse::button::r_button )
                        .set_state ( mouse::button_state::released )
                        .set_position ( glm::i32vec2 ( points.x, points.y ) );

                    mouse_button_event_.send_message ( event );
                }
            } break;
            case WM_XBUTTONDOWN:
            {
                const auto points = MAKEPOINTS ( l_param );
                const auto button = GET_XBUTTON_WPARAM ( w_param );
                if ( button == XBUTTON1 )
                {
                    if ( points.x > 0 && points.x < settings_.width_ &&
                         points.y > 0 && points.y < settings_.height_ )
                    {
                        const auto event = mouse_button_event ( )
                            .set_code ( mouse::button::side_button_1 )
                            .set_state ( mouse::button_state::pressed )
                            .set_position ( glm::i32vec2 ( points.x, points.y ) );

                        mouse_button_event_.send_message ( event );
                    }
                }
                else if ( button == XBUTTON2 )
                {
                    if ( points.x > 0 && points.x < settings_.width_ &&
                         points.y > 0 && points.y < settings_.height_ )
                    {
                        const auto event = mouse_button_event ( )
                            .set_code ( mouse::button::side_button_2 )
                            .set_state ( mouse::button_state::pressed )
                            .set_position ( glm::i32vec2 ( points.x, points.y ) );

                        mouse_button_event_.send_message ( event );
                    }
                }
            } break;
            case WM_XBUTTONUP:
            {
                const auto points = MAKEPOINTS ( l_param );
                const auto button = GET_XBUTTON_WPARAM ( w_param );
                if ( button == XBUTTON1 )
                {
                    if ( points.x > 0 && points.x < settings_.width_ &&
                         points.y > 0 && points.y < settings_.height_ )
                    {
                        const auto event = mouse_button_event ( )
                            .set_code ( mouse::button::side_button_1 )
                            .set_state ( mouse::button_state::released )
                            .set_position ( glm::i32vec2 ( points.x, points.y ) );

                        mouse_button_event_.send_message ( event );
                    }
                }
                else if ( button == XBUTTON2 )
                {
                    if ( points.x > 0 && points.x < settings_.width_ &&
                         points.y > 0 && points.y < settings_.height_ )
                    {
                        const auto event = mouse_button_event ( )
                            .set_code ( mouse::button::side_button_2 )
                            .set_state ( mouse::button_state::released )
                            .set_position ( glm::i32vec2 ( points.x, points.y ) );

                        mouse_button_event_.send_message ( event );
                    }
                }
            } break;
            case WM_MOUSEMOVE:
            {
                const auto points = MAKEPOINTS ( l_param );

                if ( points.x > 0 && points.x < settings_.width_ && 
                     points.y > 0 && points.y < settings_.height_ )
                {
                    const auto event = mouse_motion_event ( )
                        .set_position ( glm::i32vec2( points.x , points.y ) );

                    mouse_motion_event_.send_message ( event );
                }
            } break;
            case WM_MOUSEWHEEL:
            {
                const auto points = MAKEPOINTS ( l_param );
                if( GET_WHEEL_DELTA_WPARAM ( w_param ) > 0 )
                {
                    const auto event = mouse_button_event ( )
                        .set_code ( mouse::button::scroll_up )
                        .set_position ( glm::i32vec2 { static_cast< int32_t >( points.x ), static_cast< int32_t >( points.y ) } );

                    mouse_button_event_.send_message ( event );
                }
                else if( GET_WHEEL_DELTA_WPARAM ( w_param ) < 0 )
                {
                    const auto event = mouse_button_event ( )
                        .set_code ( mouse::button::scroll_down )
                        .set_position ( glm::i32vec2 { static_cast< int32_t >( points.x ), static_cast< int32_t >( points.y ) } );


                    mouse_button_event_.send_message ( event );
                }
            } break;
            case WM_MOUSEHWHEEL:
            {
                const auto points = MAKEPOINTS ( l_param );
                if ( GET_WHEEL_DELTA_WPARAM ( w_param ) > 0 )
                {
                    const auto event = mouse_button_event ( )
                        .set_code ( mouse::button::scroll_left )
                        .set_position ( glm::i32vec2 { static_cast< int32_t >( points.x ), static_cast< int32_t >( points.y ) } );

                    mouse_button_event_.send_message ( event );
                }
                else if ( GET_WHEEL_DELTA_WPARAM ( w_param ) < 0 )
                {
                    const auto event = mouse_button_event ( )
                        .set_code ( mouse::button::scroll_right )
                        .set_position ( glm::i32vec2 { static_cast< int32_t >( points.x ), static_cast< int32_t >( points.y ) } );

                    mouse_button_event_.send_message ( event );
                }
            } break;
            case WM_MOVE:
            {
                settings_.x_ = LOWORD ( l_param );
                settings_.y_ = HIWORD ( l_param );
            } break;
            case WM_SIZE:
            {
                if ( settings_.width_ != LOWORD ( l_param ) || 
                     settings_.height_ != HIWORD ( l_param ) )
                {
                    settings_.width_ = LOWORD ( l_param );
                    settings_.height_ = HIWORD ( l_param );

                    const auto event = framebuffer_resize_event ( )
                        .set_size ( glm::i32vec2 { settings_.width_, settings_.height_ } );

                    framebuffer_resize_event_.send_message ( event );
                }
            } break;

        }

        return DefWindowProc ( h_wnd, msg, w_param, l_param );
    }
}

#endif