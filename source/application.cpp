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

#include <application.h>

#include "application.h"

#if defined( VK_USE_PLATFORM_WIN32_KHR )
#include "window/win32_window.h"
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
#elif defined( VK_USE_PLATFORM_XCB_KHR )
#include "window/xcb_window.h"
#endif

#include "TWE.h"

namespace TWE
{
    application::application ( const std::string& title )
    {
#if defined( VK_USE_PLATFORM_WIN32_KHR )
        p_wnd_ = std::make_unique<win32_window> ( );
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
        p_wnd_ = std::make_unique<wayland_window> ( );
#elif defined( VK_USE_PLATFORM_XCB_KHR )
        p_wnd_ = std::make_unique<xcb_window> ( title );
#endif
        
        p_renderer_ = std::make_unique<renderer>( p_wnd_.get(), p_wnd_->get_title(), VK_MAKE_VERSION( 0, 0, 1 ) );
    }
    application::~application ( )
    {

    }

    void application::run ( )
    {
        while( p_wnd_->is_open() )
        {
            p_wnd_->poll_events();
            
            p_renderer_->draw_frame();
        }
    }
    
    void application::init_graphics_pipeline( const std::string &vertex_shader_filepath,
        const std::string &fragment_shader_filepath )
    {
        p_renderer_->setup_graphics_pipeline( { vertex_shader_filepath, fragment_shader_filepath } );
        p_renderer_->record_draw_calls();
    }
}