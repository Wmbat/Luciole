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

#ifndef TWE_EVENT_DISPATCHER_H
#define TWE_EVENT_DISPATCHER_H

#include <vector>
#include <memory>

#include "event.h"
#include "event_listeners.h"
#include "../../TWE_core.h"

namespace TWE
{
    class event_dispatcher
    {
    public:
        void TWE_API add_key_listener(
            const std::shared_ptr<i_key_listener>& sp_listener );
        void TWE_API remove_key_listener(
            const std::shared_ptr<i_key_listener>& sp_listener );
        
        void TWE_API add_mouse_button_listener(
            const std::shared_ptr<i_mouse_button_listener>& sp_listener );
        void TWE_API remove_mouse_button_listener(
            const std::shared_ptr<i_mouse_button_listener>& sp_listener );
        
        void TWE_API add_mouse_motion_listener(
            const std::shared_ptr<i_mouse_motion_listener>& sp_listener );
        void TWE_API remove_mouse_motion_listener(
            const std::shared_ptr<i_mouse_motion_listener>& sp_listener );
        
        void TWE_API add_window_close_listener(
            const std::shared_ptr<i_window_close_listener>& sp_listener );
        void TWE_API remove_window_close_listener(
            const std::shared_ptr<i_window_close_listener>& sp_listener );
        
        void TWE_API add_framebuffer_resize_listener(
            const std::shared_ptr<i_framebuffer_resize_listener>& sp_listener );
        void TWE_API remove_framebuffer_resize_listener(
            const std::shared_ptr<i_framebuffer_resize_listener>& sp_listener );
        
    protected:
    
        /*!
         * @brief Pass a key_event to all i_key_event_listeners present
         * in the std::vector wp_key_event_listeners_.
         * Should be called by the inheritor of the event_dispacher_class.
         * @param event The event to pass to all wp_key_event_listeners present
         * in the std::vector of weak_ptr.
         */
        void TWE_API dispatch_key_event( const key_event& event );
        /*!
         * @brief Pass a mouse_button_event to all i_mouse_button_listeners
         * present in the std::vector wp_mouse_button_listeners_.
         * Should be called by the inheritor of the event_dispatcher class.
         * @param event The event to pass to all wp_mouse_button_listeners_ present
         * in the std::vector of weak_ptr.
         */
        void TWE_API dispatch_mouse_button_event( const mouse_button_event& event );
        /*!
         * @brief Pass a mouse_motion_event to all i_mouse_motion_listeners
         * present in the std::vector wp_mouse_motion_listeners_.
         * Should be called by the inheritor of the event_dispatcher class.
         * @param event The event to pass to all wp_mouse_motion_listeners present
         * in the std::vector of weak_ptr
         */
        void TWE_API dispatch_mouse_motion_event( const mouse_motion_event& event );
        /*!
         * @brief Pass a window_close_event to all i_window_close_listeners
         * present in the std::vector wp_window_close_listeners_.
         * Should be called by the inheritor of the event_dispatcher class.
         * @param event The event to pass to all wp_window_close_listeners present
         * in the std::vector of weak_ptr.
         */
        void TWE_API dispatch_window_close_event ( const window_close_event& event );
        /*!
         * @brief Pass a framebuffer_resize_event to all i_framebuffer_resize_listeners
         * present in the std::vector wp_framebuffer_resize_listeners_
         * Should be called by the inheritor of the event_dispatcher class.
         * @param event The event to pass to all wp_framebuffer_resize_listeners_
         * present in the std::vector weak_ptr.
         */
        void TWE_API dispatch_framebuffer_resize_event( const framebuffer_resize_event& event );
    
    private:
        std::vector<std::weak_ptr<i_key_listener>> wp_key_listeners_;
        std::vector<std::weak_ptr<i_mouse_button_listener>> wp_mouse_button_listeners_;
        std::vector<std::weak_ptr<i_mouse_motion_listener>> wp_mouse_motion_listeners_;
        std::vector<std::weak_ptr<i_window_close_listener>> wp_window_close_listeners_;
        std::vector<std::weak_ptr<i_framebuffer_resize_listener>> wp_framebuffer_resize_listeners_;
    };
}

#endif //TWE_EVENT_DISPATCHER_H
