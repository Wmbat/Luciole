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

#ifndef TWE_I_EVENT_LISTENER_H
#define TWE_I_EVENT_LISTENER_H

#include "event.h"

namespace TWE
{
    /*!
     * @brief An interface to allow for the listening to a
     * key event dispatched by an event dispatcher.
     */
    class i_key_listener
    {
    public:
        /*!
         * @brief Virtual function to be overriden by the inheritor of
         * the interface. It allows for each listener to have a different
         * behaviour for a single event.
         * @public
         * @param event -> The event received from the dispatcher.
         */
        virtual void on_key_event( const key_event& event ) = 0;
    };
    
    /*!
     * @brief An interface to allow for the listening to mouse
     * button events dispatched by an event dispatcher.
     */
    class i_mouse_button_listener
    {
    public:
        /*!
         * @brief A Virtual function to be overriden by the inheritor of
         * the interface, It allows for each listener to have a different
         * behaviour for the event.
         * @public
         * @param event -> The event received from the dispatcher.
         */
        virtual void on_mouse_button_event( const mouse_button_event& event ) = 0;
    };
    
    
    /*!
     * @brief An interface to allow for the listening to mouse motion
     * on the window from an event dispatcher
     */
    class i_mouse_motion_listener
    {
    public:
        /*!
         * @brief
         * @param event -> The event received from the dispatcher
         */
        virtual void on_mouse_motion( const mouse_motion_event& event ) = 0;
    };

    class i_window_close_listener
    {
    public:
        virtual void on_window_close ( const window_close_event& event ) = 0;
    };
    
     class i_framebuffer_resize_listener
     {
     public:
         virtual void on_framebuffer_resize( const framebuffer_resize_event& event ) = 0;
     };
}
#endif //TWE_I_EVENT_LISTENER_H
