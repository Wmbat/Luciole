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

#include "window/win32_window.h"

#include "log.h"

#if defined( VK_USE_PLATFORM_WIN32_KHR )
namespace TWE
{
    static inline LRESULT CALLBACK window_proc ( HWND h_wnd, UINT message, WPARAM w_param, LPARAM l_param )
    {
        switch( message )
        {
            case WM_DESTROY:
            {
                PostQuitMessage ( 0 );
                return 0;
            } break;
        }

        return DefWindowProc ( h_wnd, message, w_param, l_param );
    }

    win32_window::win32_window ( const std::string& title )
    {
        core_info ( "Using Win32 for window creation." );

        title_ = title;
        open_ = true;

        win_instance_ = GetModuleHandle ( NULL );

        WNDCLASSEX wc
        {
            sizeof ( WNDCLASSEX ),              // cbSize
            CS_HREDRAW | CS_VREDRAW,            // style
            window_proc,                        // lpfnWndProc
            0,                                  // cbClsExtra
            0,                                  // cbWndExtra
            win_instance_,                      // hInstance
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

        win_window_ = CreateWindow ( wnd_class_name_, title_.c_str ( ),
                                     WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                                     settings_.x_, settings_.y_,
                                     wnd_rect.right - wnd_rect.left,
                                     wnd_rect.bottom - wnd_rect.top,
                                     nullptr, nullptr, win_instance_, this );

        if( win_window_ == nullptr )
        {
            UnregisterClass ( wnd_class_name_, win_instance_ );
        }

        ShowWindow ( win_window_, SW_SHOWDEFAULT );
        UpdateWindow ( win_window_ );
    }
    win32_window::win32_window ( win32_window&& rhs ) noexcept
    {
        *this = std::move ( rhs );
    }
    win32_window::~win32_window ( )
    {
        UnregisterClass ( wnd_class_name_, win_instance_ );
    }

    win32_window& win32_window::operator=( win32_window&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            title_ = rhs.title_;
            rhs.title_ = { };

            open_ = rhs.open_;
            rhs.open_ = false;

            win_instance_ = rhs.win_instance_;
            rhs.win_instance_ = nullptr;

            win_window_ = rhs.win_window_;
            rhs.win_window_ = nullptr;

            event_handler_ = rhs.event_handler_;
            rhs.event_handler_ = { };

            settings_ = rhs.settings_;
            rhs.settings_ = { };
        }

        return *this;
    }

    void win32_window::poll_events ( )
    {

    };

    vk_return_type<VkSurfaceKHR> win32_window::create_surface ( const VkInstance& instance ) const noexcept
    {
        VkSurfaceKHR surface;

        const VkWin32SurfaceCreateInfoKHR create_info
        {
            VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,            // sType
            nullptr,                                                    // pNext
            { },                                                        // flags
            win_instance_,                                              // hinstance
            win_window_                                                 // hwnd
        };

        return { vkCreateWin32SurfaceKHR ( instance, &create_info, nullptr, &surface ), surface };
    }
}
#endif