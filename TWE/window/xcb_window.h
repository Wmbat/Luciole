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

#ifndef XCB_WINDOW_H
#define XCB_WINDOW_H

#include <memory>
#include <functional>

#if defined( VK_USE_PLATFORM_XCB_KHR )
#include <xcb/xcb.h>

#include "base_window.h"

namespace TWE
{
    class xcb_window : public base_window
    {
    public:
        TWE_API xcb_window( ) = default;
        TWE_API explicit xcb_window( const std::string& title );
        TWE_API xcb_window( const xcb_window& rhs ) = delete;
        TWE_API xcb_window( xcb_window&& rhs ) noexcept;
        TWE_API ~xcb_window( );
        
        TWE_API xcb_window& operator=( const xcb_window& rhs ) = delete;
        TWE_API xcb_window& operator=( xcb_window&& rhs ) noexcept;
    
        virtual void TWE_API poll_events( ) override;
        
        vk_return_type<VkSurfaceKHR> TWE_API create_surface( const VkInstance& instance ) const noexcept override;
    
    private:
        std::unique_ptr<xcb_connection_t, std::function<void( xcb_connection_t* )>> p_xcb_connection_;
        xcb_screen_t* p_xcb_screen_;
        xcb_window_t xcb_window_;
    
        std::unique_ptr<xcb_intern_atom_reply_t> p_xcb_wm_delete_window_;
    };
}
#endif

#endif // XCB_WINDOW_H