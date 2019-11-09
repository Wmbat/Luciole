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

#ifndef LUCIOLE_UI_WINDOW_HPP
#define LUCIOLE_UI_WINDOW_HPP

/* INCLUDES */
#include <luciole/luciole_core.hpp>
#include <luciole/ui/event.hpp>
#include <luciole/utils/delegate.hpp>
#include <luciole/utils/message_handler.hpp>
#include <luciole/vk/core.hpp>
#include <luciole/vk/errors.hpp>

#include <glm/vec2.hpp>
#include <vulkan/vulkan.h>
#include <spdlog/logger.h>

#if defined( VK_USE_PLATFORM_XCB_KHR )
#include <xcb/xcb.h>
#endif

#include <string>
#include <variant>
#include <memory>

namespace ui
{
class window
{
private:
   /**
    * @brief Dummy struct for the definition of custom strong_types
    * in the window class.
    */
   struct window_param {};

public:
#if defined( VK_USE_PLATFORM_XCB_KHR )
   using xcb_connection_uptr = std::unique_ptr<xcb_connection_t, delegate<void( xcb_connection_t* )>>;
   using xcb_intern_atom_uptr = std::unique_ptr<xcb_intern_atom_reply_t, delegate<void(xcb_intern_atom_reply_t*)>>; 
#endif

   /**
    * @brief Forward declaration of the create_info struct.
    */
   struct create_info;

   using create_info_t = strong_type<create_info const&, window_param>;

public:
   window() = default;
   window( create_info_t const& create_info );
   window( window const& wnd ) = delete;
   window( window&& wnd );
   ~window();

   window& operator=( window const& rhs ) = delete;
   window& operator=( window&& rhs );

   /**
    * @brief Check if the window is open.
    * 
    * @return true if the the window is open.
    * @return false if the window is clored.
    */
   bool is_open( );

   /**
    * @brief Pools the window for various events such as:
    * - Key events
    * - Mouse events,
    * - Window Close Events.
    * - Window Resize Events.
    */
   void poll_events();

   /**
    * @brief Create a surface object
    * 
    * @param instance The Vulkan instance used for the creation of the VkSurfaceKHR.
    * @return std::variant<VkSurfaceKHR, vk::error::type> 
    */
   [[nodiscard]] 
   std::variant<VkSurfaceKHR, vk::error> create_surface( 
      vk::instance_t instance 
   ) const PURE;

   /**
    * @brief Add a callback function to be called when a key event is triggered. 
    * 
    * @tparam C The type of the function.
    * @param callback The function to add to the key event messenger.
    * @return std::enable_if_t<std::is_same_v<C, key_event_delg>, void> Returns void.
    */
   template<class C>
   std::enable_if_t<std::is_same_v<C, key_event_delg>, void> add_callback ( const C& callback )
   {
      key_handler.add_callback( callback );
   }

   /**
    * @brief Add a callback function to be called when a mouse button event is triggered. 
    * 
    * @tparam C The type of the function.
    * @param callback The function to add to the mouse button event messenger.
    * @return std::enable_if_t<std::is_same_v<C, mouse_button_event_delg>, void>  Return void.
    */
   template<class C>
   std::enable_if_t<std::is_same_v<C, mouse_button_event_delg>, void> add_callback( const C& callback )
   {
      mouse_button_handler.add_callback( callback );
   }

   /**
    * @brief Add a callback function to be called when a mouse motion event is triggered. 
    * 
    * @tparam C The type of the function.
    * @param callback The function to add to the mouse motion event messenger.
    * @return std::enable_if_t<std::is_same_v<C, mouse_motion_event_delg>, void> Returns void.
    */
   template<class C>
   std::enable_if_t<std::is_same_v<C, mouse_motion_event_delg>, void> add_callback( const C& callback )
   {
      mouse_motion_handler.add_callback( callback );
   }

   /**
    * @brief Add a callback function to be called when a window close event is triggered. 
    * 
    * @tparam C The type of the function.
    * @param callback The function to add to the window close event messenger.
    * @return std::enable_if_t<std::is_same_v<C, window_close_event_delg>, void> Returns void.
    */
   template<class C>
   std::enable_if_t<std::is_same_v<C, window_close_event_delg>, void> add_callback( const C& callback )
   {
      window_close_handler.add_callback( callback );
   }

   /**
    * @brief Add a callback function to be called when a framebuffer resize event is triggered. 
    * 
    * @tparam C The type of the function.
    * @param callback The function to add to the framebuffer resize event messenger.
    * @return std::enable_if_t<std::is_same_v<C, framebuffer_resize_event_delg>, void> Returs void.
    */
   template<class C>
   std::enable_if_t<std::is_same_v<C, framebuffer_resize_event_delg>, void> add_callback( const C& callback )
   {
      framebuffer_resize_handler.add_callback( callback );
   }

   [[nodiscard]] 
   glm::uvec2 get_size() const PURE;

private:
   std::string title;
   glm::uvec2 position;
   glm::uvec2 size;

   bool is_wnd_open;
   bool is_fullscreen;

#if defined( VK_USE_PLATFORM_XCB_KHR )
   xcb_connection_uptr p_xcb_connection;
   xcb_screen_t* p_xcb_screen;
   xcb_window_t xcb_window;
       
   xcb_intern_atom_uptr p_xcb_wm_delete_window;

   int default_screen_id;
#endif

   std::shared_ptr<spdlog::logger> window_logger;

   message_handler<const key_event> key_handler;
   message_handler<const mouse_button_event> mouse_button_handler;
   message_handler<const mouse_motion_event> mouse_motion_handler;
   message_handler<const window_close_event> window_close_handler;
   message_handler<const framebuffer_resize_event> framebuffer_resize_handler;

public:
   /**
    * @brief Use to pass creation info the the window. 
    */
   struct create_info
   {
      std::string title = "Default Luciole Window";
      glm::uvec2 position = { 100, 100 };
      glm::uvec2 size = { 1080, 720 };
   }; // class struct
}; // class window    
} // namespace ui

#endif // LUCIOLE_UI_WINDOW_HPP
