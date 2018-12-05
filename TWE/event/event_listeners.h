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
     * @brief An interface to allow for listening to
     * key press events dispatched by an event dispatcher.
     */
    class key_pressed_listener
    {
    public:
        /*!
         * @brief Virtual function to be overriden the by inheritor to
         * define the desired behaviour at the key press.
         * @param event The event to handle.
         */
        virtual void execute( const key_press_event& event ) = 0;
    };
    
    /*!
     * @brief An interface that allows for listening to
     * key release events dispatched by an event dispatcher.
     */
    class key_released_listener
    {
    public:
        /*!
         * @brief Virtual function to be overriden the by inheritor to
         * define the desired behaviour at the key release.
         * @param event The event to handle.
         */
        virtual void execute( const key_release_event& event ) = 0;
    };
    
    /*!
     * @brief An interface that allows for listening to
     * mouse button press events dispatched by an event
     * dispatcher.
     */
    class mouse_button_pressed_listener
    {
    public:
        /*!
         * @brief Virtual function to be overriden by the inheritor to
         * define the desired behaviour at the mouse button press.
         * @param event The event to handle.
         */
        virtual void execute( const mouse_button_press_event& event ) = 0;
    };
    
    /*!
     * @brief An interface that allows for listening to
     * mouse button release events dispatched by an event
     * dispatcher.
     */
    class mouse_button_released_listener
    {
    public:
        /*!
         * @brief Virtual function to be overriden by the inheritor to
         * define the desired behaviour at the mouse button release.
         * @param event The event to handle.
         */
        virtual void execute( const mouse_button_release_event& event ) = 0;
    };
    
    /*!
     * @brief An interface that allows for listening to
     * mouse motion events dispatched by an event dispatcher.
     */
    class mouse_motion_listener
    {
    public:
        /*!
         * @brief Virtual function to be overriden by the inheritor to
         * define the desired behaviour at the mouse motion event
         * @param event The event to handle.
         */
        virtual void execute( const mouse_motion_event& event ) = 0;
    };

    /*!
     * @brief An interface that allows for listener to
     * windown close events dispatched by an event dispatcher.
     */
    class window_close_listener
    {
    public:
        /*!
         * @brief Virtual to function to be overriden by the inheritor to
         * define the desired behaviour when the window close event is launched.
         * @param event The event to handle.
         */
        virtual void execute ( const window_close_event& event ) = 0;
    };
    
    /*!
     * @brief An interface that allows for listening to
     * framebuffer resize events dispatched by an event dispatcher.
     */
     class framebuffer_resize_listener
     {
     public:
         /*!
          * @brief Virtual function to be overriden by the inheritor to
          * define the desired behaviour at the framebuffer resize event
          * @param event The event to handle.
          */
         virtual void execute( const framebuffer_resize_event& event ) = 0;
     };
}
#endif //TWE_I_EVENT_LISTENER_H
