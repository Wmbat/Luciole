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
    void event_dispatcher::add_key_pressed_listener(
        const std::shared_ptr<key_pressed_listener>& p_listener )
    {
        /* Find if listener is not already in vector. */
        if ( key_press_listeners_.cend( ) ==
             std::find_if( key_press_listeners_.cbegin( ),key_press_listeners_.cend( ),
                 [p_listener]( const std::shared_ptr<key_pressed_listener> &p_lis )
                 { return p_listener == p_lis; } ) )
        {
            key_press_listeners_.emplace_back( p_listener );
        }
    }
    void event_dispatcher::add_key_released_listener(
        const std::shared_ptr<key_released_listener>& p_listener )
    {
        /* Find if listener is not already in vector. */
        if ( key_release_listeners_.cend( ) ==
             std::find_if( key_release_listeners_.cbegin( ),key_release_listeners_.cend( ),
                 [p_listener]( const std::shared_ptr<key_released_listener> &p_lis )
                 { return p_listener == p_lis; } ) )
        {
            key_release_listeners_.emplace_back( p_listener );
        }
    }
    void event_dispatcher::add_mouse_button_pressed_listener(
        const std::shared_ptr<mouse_button_pressed_listener>& p_listener )
    {
        /* Find if listener is not already in vector. */
        if( mouse_button_press_listeners_.cend( ) ==
            std::find_if( mouse_button_press_listeners_.cbegin(), mouse_button_press_listeners_.cend(),
                [p_listener]( const std::shared_ptr<mouse_button_pressed_listener>& p_lis)
                { return p_listener == p_lis; } ) )
        {
            mouse_button_press_listeners_.emplace_back( p_listener );
        }
    }
    void event_dispatcher::add_mouse_button_released_listener(
        const std::shared_ptr<mouse_button_released_listener>& p_listener )
    {
        /* Find if listener is not already in vector. */
        if( mouse_button_release_listeners_.cend( ) ==
            std::find_if( mouse_button_release_listeners_.cbegin(), mouse_button_release_listeners_.cend(),
                [p_listener]( const std::shared_ptr<mouse_button_released_listener>& p_lis )
                { return p_listener == p_lis; } ) )
        {
            mouse_button_release_listeners_.emplace_back( p_listener );
        }
    }
    void event_dispatcher::add_mouse_motion_listener(
        const std::shared_ptr<mouse_motion_listener>& p_listener )
    {
        /* Find if listener is not already in vector. */
        if( mouse_motion_listeners_.cend( ) ==
            std::find_if( mouse_motion_listeners_.cbegin(), mouse_motion_listeners_.cend(),
                [p_listener]( const std::shared_ptr<mouse_motion_listener>& p_list )
                { return p_listener == p_list; } ) )
        {
            mouse_motion_listeners_.emplace_back( p_listener );
        }
    }
    
    
    void event_dispatcher::remove_key_pressed_listener( const std::shared_ptr<key_pressed_listener>& p_listener )
    {
        /* Find if p_listeners in present */
        const auto it = std::find_if( key_press_listeners_.cbegin( ),key_press_listeners_.cend( ),
            [p_listener]( const std::shared_ptr<key_pressed_listener> &p_lis )
            { return p_listener == p_lis; } );
    
        /* If present, remove it */
        if( it != key_press_listeners_.cend() )
        {
            key_press_listeners_.erase( it );
        }
    }
    void event_dispatcher::remove_key_released_listener( const std::shared_ptr<key_released_listener>& p_listener )
    {
        /* Find if p_listeners in present */
        const auto it = std::find_if( key_release_listeners_.cbegin( ),key_release_listeners_.cend( ),
            [p_listener]( const std::shared_ptr<key_released_listener> &p_lis )
            { return p_listener == p_lis; } );
    
        /* If present, remove it */
        if( it != key_release_listeners_.cend() )
        {
            key_release_listeners_.erase( it );
        }
    }
    void event_dispatcher::remove_mouse_button_pressed_listener(
        const std::shared_ptr<mouse_button_pressed_listener>& p_listener )
    {
        /* Find if p_listeners in present */
        const auto it = std::find_if( mouse_button_press_listeners_.cbegin(), mouse_button_press_listeners_.cend(),
            [p_listener]( const std::shared_ptr<mouse_button_pressed_listener>& p_list )
            { return p_listener == p_list;} );
    
        /* If present, remove it */
        if( it != mouse_button_press_listeners_.cend() )
        {
            mouse_button_press_listeners_.erase( it );
        }
    }
    void event_dispatcher::remove_mouse_button_released_listener(
        const std::shared_ptr<mouse_button_released_listener>& p_listeners )
    {
        /* Find if p_listeners in present */
        const auto it = std::find_if( mouse_button_release_listeners_.cbegin( ), mouse_button_release_listeners_.cend(),
            [p_listeners]( const std::shared_ptr<mouse_button_released_listener>& p_list )
            { return p_listeners == p_list;} );
    
        /* If present, remove it */
        if( it != mouse_button_release_listeners_.cend() )
        {
            mouse_button_release_listeners_.erase( it );
        }
    }
    void event_dispatcher::remove_mouse_motion_listener(
        const std::shared_ptr<mouse_motion_listener>& p_listeners )
    {
        /* Find if p_listeners in present */
        const auto it = std::find_if( mouse_motion_listeners_.cbegin(), mouse_motion_listeners_.cend(),
            [p_listeners]( const std::shared_ptr<mouse_motion_listener>& p_list )
            { return p_listeners == p_list; } );
        
        /* If present, remove it */
        if( it != mouse_motion_listeners_.cend( ) )
        {
            mouse_motion_listeners_.erase( it );
        }
    }
    
    void event_dispatcher::dispatch_key_pressed_event( const key_press_event& event )
    {
        for( auto& p_listener : key_press_listeners_ )
        {
            p_listener->on_key_press( event );
        }
    }
    void event_dispatcher::dispatch_key_released_event( const key_release_event& event )
    {
        for( auto& p_listener : key_release_listeners_ )
        {
            p_listener->on_key_release( event );
        }
    }
    void event_dispatcher::dispatch_mouse_button_pressed_event( const mouse_button_press_event& event )
    {
        for( auto& p_listener : mouse_button_press_listeners_ )
        {
            p_listener->on_button_press( event );
        }
    }
    void event_dispatcher::dispatch_mouse_button_released_event( const mouse_button_release_event& event )
    {
        for( auto& p_listener : mouse_button_release_listeners_ )
        {
            p_listener->on_button_release( event );
        }
    }
    void event_dispatcher::dispatch_mouse_motion_event( const mouse_motion_event& event )
    {
        for( auto& p_listener : mouse_motion_listeners_ )
        {
            p_listener->on_mouse_motion( event );
        }
    }
}