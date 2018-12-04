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
#include "../TWE_core.h"

namespace TWE
{
    class event_dispatcher
    {
    public:
        /*!
         * @brief Add a shared_ptr to an object that inherits from
         * key_pressed_listener to listen for key presses.
         * @param p_listener A shared_ptr to the object that inherits
         * from key_pressed_listener.
         */
        void TWE_API add_key_pressed_listener(
            const std::shared_ptr<key_pressed_listener>& p_listener );
        /*!
         * @brief Add a shared_ptr to an object that inherits from
         * key_released_listener to listen for key releases.
         * @param p_listener A shared_ptr to the object that inherits from
         * key_released_listener.
         */
        void TWE_API add_key_released_listener(
            const std::shared_ptr<key_released_listener>& p_listener );
        /*!
         * @brief Add a shared_ptr to the object that inherits from
         * mouse_button_pressed_listener to listen for mouse button presses.
         * @param p_listener A shared_ptr to the object that inherits from
         * mouse_button_pressed_listener.
         */
        void TWE_API add_mouse_button_pressed_listener(
            const std::shared_ptr<mouse_button_pressed_listener>& p_listener );
        /*!
         * @brief Add a shared_ptr to an object that inherits from
         * mouse_button_released_listener to listen for mouse button releases.
         * @param p_listener A shared_ptr to the object that inherits from
         * mouse_button_released_listener.
         */
        void TWE_API add_mouse_button_released_listener(
            const std::shared_ptr<mouse_button_released_listener>& p_listener );
        /*!
         * @brief Add a shared_ptr to an object that inherits from
         * mouse_motion_listener to listen for mouse motion.
         * @param p_listener A shared_ptr to the object that inherits from
         * mouse_motion_listener.
         */
        void TWE_API add_mouse_motion_listener(
            const std::shared_ptr<mouse_motion_listener>& p_listener );
        /*!
         * @brief Add a shared_ptr to an object that inherits from
         * framebuffer_resize_listener to listen for framebuffer resizes.
         * @param p_listener A shared_ptr to the object that inherits from
         * framebuffer_resize_listener.
         */
        void TWE_API add_framebuffer_resize_listener(
            const std::shared_ptr<framebuffer_resize_listener>& p_listener );
        
        
        /*!
         * @brief Remove a key pressed listener from the vector of
         * listeners.
         * @param p_listener The shared_ptr to a key pressed listener
         * that is to be removed if present in the key pressed listener
         * vector.
         */
        void TWE_API remove_key_pressed_listener(
            const std::shared_ptr<key_pressed_listener>& p_listener );
        /*!
         * @brief Remove a key released listener from the vector of
         * listeners.
         * @param p_listener The shared_ptr to a key released listener
         * that is to be removed if present in the key released listener
         * vector.
         */
        void TWE_API remove_key_released_listener(
            const std::shared_ptr<key_released_listener>& p_listener );
        /*!
         * @brief Remove a mouse button pressed listener from the vector of
         * listeners.
         * @param p_listener The shared_ptr to a mouse_button_pressed_listener
         * that is to be removed if present in the mouse button pressed listener
         * vector.
         */
        void TWE_API remove_mouse_button_pressed_listener(
            const std::shared_ptr<mouse_button_pressed_listener>& p_listener );
        /*!
         * @brief Remove a mouse button released listener from the vector of
         * listeners.
         * @param p_listeners The shared_ptr to a mouse_button_released_listener
         * that is to be removed if present in the mouse button pressed listener
         * vector.
         */
        void TWE_API remove_mouse_button_released_listener(
            const std::shared_ptr<mouse_button_released_listener>& p_listener );
        /*!
         * @brief Remove a mouse motion listener from the vector of
         * listeners.
         * @param p_listeners The shared_ptr to a mouse_motion_listener
         * that is to be removed if present in the mouse motion listener
         * vector.
         */
        void TWE_API remove_mouse_motion_listener(
            const std::shared_ptr<mouse_motion_listener>& p_listener );
        /*!
         * @brief Remove a framebuffer resize listener from the vector
         * of listeners.
         * @param p_listener The shared_ptr to a framebuffer_resize_listener
         * that is to be removed if present in the vector.
         */
        void TWE_API remove_framebuffer_resize_listener(
            const std::shared_ptr<framebuffer_resize_listener>& p_listener );
        
    protected:
        /*!
         * @brief Dispatch a key press event to all the
         * key press event listeners.
         * @param event The key press event to dispatch.
         */
        void TWE_API dispatch_key_pressed_event( const key_press_event& event );
        /*!
         * @brief Dispatch a key release event to all the
         * key release event listeners.
         * @param event The key release event to dispatch.
         */
        void TWE_API dispatch_key_released_event( const key_release_event& event );
        /*!
         * @brief Dispatch a mouse button press event to all the
         * mouse button press event listeners.
         * @param event The mouse button press event to dispatch.
         */
        void TWE_API dispatch_mouse_button_pressed_event( const mouse_button_press_event& event );
        /*!
         * @brief Dispatch a mouse button released even to all the
         * mouse button release event listeners.
         * @param event The mouse button release event to dispatch.
         */
        void TWE_API dispatch_mouse_button_released_event( const mouse_button_release_event& event );
        /*!
         * @brief Dispatch the mouse motion event to all the
         * mouse motion event listeners.
         * @param event The mouse motion event to dispatch.
         */
        void TWE_API dispatch_mouse_motion_event( const mouse_motion_event& event );
        /*!
         * @brief Dispatch the framebuffer resize event to all the
         * framebuffer resize event listeners.
         * @param event The framebuffer resize event to dispatch.
         */
        void TWE_API dispatch_framebuffer_resize_event( const framebuffer_resize_event& event );
    
    private:
        /* vector of key_press_listeners. */
        std::vector<std::shared_ptr<key_pressed_listener>> key_press_listeners_;
        /* vector of key_release_listeners. */
        std::vector<std::shared_ptr<key_released_listener>> key_release_listeners_;
        /* vector of mouse_button_pressed_listeners. */
        std::vector<std::shared_ptr<mouse_button_pressed_listener>> mouse_button_press_listeners_;
        /* vector of mouse_button_released_listeners. */
        std::vector<std::shared_ptr<mouse_button_released_listener>> mouse_button_release_listeners_;
        /* vector of mouse_motion_listeners. */
        std::vector<std::shared_ptr<mouse_motion_listener>> mouse_motion_listeners_;
        /* vector of framebuffer resize listeners. */
        std::vector<std::shared_ptr<framebuffer_resize_listener>> framebuffer_resize_listeners_;
    };
}

#endif //TWE_EVENT_DISPATCHER_H
