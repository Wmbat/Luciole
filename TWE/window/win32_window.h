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

#ifndef WIN32_WINDOW_H
#define WIN32_WINDOW_H

#include "base_window.h"

#if defined( VK_USE_PLATFORM_WIN32_KHR )

namespace TWE
{
    class win32_window : public base_window
    {
    public:
        TWE_API win32_window ( ) = default;
        TWE_API explicit win32_window ( const std::string& title );
        TWE_API win32_window ( const win32_window& rhs ) = delete;
        TWE_API win32_window ( win32_window&& rhs ) noexcept;
        TWE_API ~win32_window ( );

        TWE_API win32_window& operator=( const win32_window& rhs ) = delete;
        TWE_API win32_window& operator=( win32_window&& rhs ) noexcept;

        void TWE_API poll_events ( ) override;

        vk_return_type<VkSurfaceKHR> TWE_API create_surface ( const VkInstance& instance ) const noexcept override;

    private:
        static LRESULT WINAPI handle_msg_setup ( HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param );
        static LRESULT WINAPI handle_msg_thunk ( HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param );
        LRESULT handle_msg ( HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param );

    private:
        static constexpr char* wnd_class_name_ = "TWE window";
        HINSTANCE h_inst_ = nullptr;
        HWND h_wnd_ = nullptr;
    };
}
#endif

#endif // WIN32_WINDOW_H