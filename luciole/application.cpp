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

#include <map>
#include <set>

#include <wmbats_bazaar/file_io.hpp>

#include "application.hpp"

#include "utilities/log.hpp"

#if defined( VK_USE_PLATFORM_WIN32_KHR )
#include "window/win32_window.hpp"
#elif defined( VK_USE_PLATFORM_XCB_KHR )
#include "window/xcb_window.hpp"
#endif 

application::application( )
{
#if defined( VK_USE_PLATFORM_WIN32_KHR )
    p_wnd_ = std::make_unique<win32_window>( "Luciole" );
#elif defined( VK_USE_PLATFORM_XCB_KHR )
    p_wnd_ = std::make_unique<xcb_window>( "Luciole" );
#endif
        
    context_ = context( *p_wnd_.get( ) );
    renderer_ = renderer( p_context_t( &context_ ) );
}
application::~application( )
{

}

void application::run( )
{
    while( p_wnd_->is_open( ) )
    {
        renderer_.draw_frame( );

        p_wnd_->poll_events( );
    }
}