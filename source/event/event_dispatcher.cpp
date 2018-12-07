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

#include <algorithm>
#include <event/event_dispatcher.h>


#include "event/event_dispatcher.h"
#include "log.h"

namespace TWE
{
    void event_dispatcher::add_key_listener(
        const std::shared_ptr<i_key_listener>& sp_listener )
    {
        const auto it = std::find_if( wp_key_listeners_.cbegin( ), wp_key_listeners_.cend( ),
                                      [sp_listener]( const std::weak_ptr<i_key_listener>& wp_listener )
                                      { return sp_listener == wp_listener.lock( ); } );
    
        if ( it == wp_key_listeners_.cend( ))
        {
            wp_key_listeners_.emplace_back( std::weak_ptr<i_key_listener>( sp_listener ));
        }
    }
    void event_dispatcher::remove_key_listener(
        const std::shared_ptr<i_key_listener>& sp_listener )
    {
        const auto it = std::find_if( wp_key_listeners_.cbegin( ), wp_key_listeners_.cend( ),
                                      [sp_listener]( const std::weak_ptr<i_key_listener>& wp_listener )
                                      { return sp_listener == wp_listener.lock( ); } );
    
        if ( it != wp_key_listeners_.cend( ))
        {
            wp_key_listeners_.erase( it );
        }
    }

    
    
    void event_dispatcher::add_mouse_button_listener( const std::shared_ptr<TWE::i_mouse_button_listener>& sp_listener )
    {
        const auto it = std::find_if( wp_mouse_button_listeners_.cbegin( ), wp_mouse_button_listeners_.cend( ),
                                      [sp_listener]( const std::weak_ptr<i_mouse_button_listener>& wp_listener )
                                      { return sp_listener == wp_listener.lock( ); } );
    
        if ( it == wp_mouse_button_listeners_.cend( ) )
        {
            wp_mouse_button_listeners_.emplace_back( std::weak_ptr<i_mouse_button_listener>( sp_listener ));
        }
    }
    void event_dispatcher::remove_mouse_button_listener(
        const std::shared_ptr<TWE::i_mouse_button_listener>& sp_listener )
    {
        const auto it = std::find_if( wp_mouse_button_listeners_.cbegin( ), wp_mouse_button_listeners_.cend( ),
                                      [sp_listener]( const std::weak_ptr<i_mouse_button_listener>& wp_listener )
                                      { return sp_listener == wp_listener.lock( ); } );
    
        if ( it != wp_mouse_button_listeners_.cend( ) )
        {
            wp_mouse_button_listeners_.erase( it );
        }
    }

    
    void event_dispatcher::add_mouse_motion_listener(
        const std::shared_ptr<i_mouse_motion_listener>& sp_listener )
    {
        const auto it = std::find_if( wp_mouse_motion_listeners_.cbegin( ), wp_mouse_motion_listeners_.cend( ),
                                      [sp_listener]( const std::weak_ptr<i_mouse_motion_listener>& wp_listener )
                                      { return sp_listener == wp_listener.lock( ); } );
    
        if ( it == wp_mouse_motion_listeners_.cend( ))
        {
            wp_mouse_motion_listeners_.emplace_back( std::weak_ptr<i_mouse_motion_listener>( sp_listener ));
        }
    }
    void event_dispatcher::remove_mouse_motion_listener(
        const std::shared_ptr<i_mouse_motion_listener>& sp_listener )
    {
        const auto it = std::find_if( wp_mouse_motion_listeners_.cbegin( ), wp_mouse_motion_listeners_.cend( ),
                                      [sp_listener]( const std::weak_ptr<i_mouse_motion_listener>& wp_listener )
                                      { return sp_listener == wp_listener.lock( ); } );

        if ( it != wp_mouse_motion_listeners_.cend( ))
        {
            wp_mouse_motion_listeners_.erase( it );
        }
    }
    
    
    void event_dispatcher::add_window_close_listener(
        const std::shared_ptr<TWE::i_window_close_listener>& sp_listener )
    {
        const auto it = std::find_if( wp_window_close_listeners_.cbegin( ), wp_window_close_listeners_.cend( ),
                                      [sp_listener]( const std::weak_ptr<i_window_close_listener>& wp_listener )
                                      { return sp_listener == wp_listener.lock( ); } );
    
        if ( it == wp_window_close_listeners_.cend( ))
        {
            wp_window_close_listeners_.emplace_back( std::weak_ptr<i_window_close_listener>( sp_listener ));
        }
    }
    void event_dispatcher::remove_window_close_listener(
        const std::shared_ptr<TWE::i_window_close_listener>& sp_listener )
    {
        const auto it = std::find_if( wp_window_close_listeners_.cbegin( ), wp_window_close_listeners_.cend( ),
                                      [sp_listener]( const std::weak_ptr<i_window_close_listener>& wp_listener )
                                      { return sp_listener == wp_listener.lock( ); } );
    
        if ( it != wp_window_close_listeners_.cend( ))
        {
            wp_window_close_listeners_.erase( it );
        }
    }
    

    void event_dispatcher::add_framebuffer_resize_listener(
        const std::shared_ptr<TWE::i_framebuffer_resize_listener>& sp_listener )
    {
        const auto it = std::find_if( wp_framebuffer_resize_listeners_.cbegin( ),
                                      wp_framebuffer_resize_listeners_.cend( ),
                                      [sp_listener]( const std::weak_ptr<i_framebuffer_resize_listener>& wp_listener )
                                      { return sp_listener == wp_listener.lock( ); } );
    
        if ( it == wp_framebuffer_resize_listeners_.cend( ))
        {
            wp_framebuffer_resize_listeners_.emplace_back( std::weak_ptr<i_framebuffer_resize_listener>( sp_listener ));
        }
    }
    void event_dispatcher::remove_framebuffer_resize_listener(
        const std::shared_ptr<TWE::i_framebuffer_resize_listener>& sp_listener )
    {
        const auto it = std::find_if( wp_framebuffer_resize_listeners_.cbegin( ),
                                      wp_framebuffer_resize_listeners_.cend( ),
                                      [sp_listener]( const std::weak_ptr<i_framebuffer_resize_listener>& wp_listener )
                                      { return sp_listener == wp_listener.lock( ); } );
    
        if ( it != wp_framebuffer_resize_listeners_.cend( ))
        {
            wp_framebuffer_resize_listeners_.erase( it );
        }
    }
    
    
    void event_dispatcher::dispatch_key_event( const key_event& event )
    {
        for( auto& wp_listener : wp_key_listeners_ )
        {
            if( auto sp_listener = wp_listener.lock() )
            {
                sp_listener->on_key_event( event );
            }
            else
            {
                remove_key_listener( sp_listener );
            }
        }
    }
    void event_dispatcher::dispatch_mouse_button_event( const TWE::mouse_button_event& event )
    {
        for( auto& wp_listener : wp_mouse_button_listeners_ )
        {
            if( auto sp_listener = wp_listener.lock() )
            {
                sp_listener->on_mouse_button_event( event );
            }
            else
            {
                remove_mouse_button_listener( sp_listener );
            }
        }
    }
    void event_dispatcher::dispatch_mouse_motion_event( const TWE::mouse_motion_event& event )
    {
        for ( auto& wp_listener : wp_mouse_motion_listeners_ )
        {
            if( auto sp_listener = wp_listener.lock() )
            {
                sp_listener->on_mouse_motion( event );
            }
            else
            {
                remove_mouse_motion_listener( sp_listener );
            }
        }
    }
    void event_dispatcher::dispatch_window_close_event( const TWE::window_close_event& event )
    {
        for ( auto& wp_listener : wp_window_close_listeners_ )
        {
            if( auto sp_listener = wp_listener.lock() )
            {
                sp_listener->on_window_close( event );
            }
            else
            {
                remove_window_close_listener( sp_listener );
            }
        }
    }
    void event_dispatcher::dispatch_framebuffer_resize_event( const TWE::framebuffer_resize_event& event )
    {
        for( auto& wp_listener : wp_framebuffer_resize_listeners_ )
        {
            if( auto sp_listener = wp_listener.lock() )
            {
                sp_listener->on_framebuffer_resize( event );
            }
            else
            {
                remove_framebuffer_resize_listener( sp_listener );
            }
        }
    }
}