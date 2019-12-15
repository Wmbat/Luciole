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

#include <luciole/ui/window.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <algorithm>

namespace ui
{

#if defined( VK_USE_PLATFORM_XCB_KHR )
   inline window::xcb_intern_atom_uptr intern_atom_helper( xcb_connection_t *p_connection, bool only_if_exists, const std::string &str )
   {
      xcb_intern_atom_cookie_t cookie = xcb_intern_atom( p_connection, only_if_exists, static_cast<uint16_t>( str.size( ) ), str.c_str( ) );

      return window::xcb_intern_atom_uptr( xcb_intern_atom_reply( p_connection, cookie, NULL ), []( xcb_intern_atom_reply_t *ptr ) {
         free( ptr );
      } );
   }
#endif

   window::window( window::create_info_t const &create_info ) :
      title( create_info.value( ).title ), position( create_info.value( ).position ), size( create_info.value( ).size ),
      is_wnd_open( true ), is_fullscreen( false )
   {
      /* Set up window logger */
      auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>( );
      console_sink->set_pattern( "[%^%n] [thread %t] %v%$" );

      window_logger = std::shared_ptr<spdlog::logger>( new spdlog::logger( "Window Logger", {console_sink} ) );

#if defined( VK_USE_PLATFORM_XCB_KHR )
      window_logger->info( "Using XCB for window creation" );

      /** Connect to X11 window system. */
      p_xcb_connection = xcb_connection_uptr( xcb_connect( nullptr, &default_screen_id ), []( xcb_connection_t *p ) {
         xcb_disconnect( p );
      } );

      if ( xcb_connection_has_error( p_xcb_connection.get( ) ) )
      {
         window_logger->error( "Failed to connecte to the X server."
                               "\nDisconnecting from X server.\nExiting Application." );

         p_xcb_connection.reset( );
      }
      else
      {
         window_logger->info( "Connection to X server established" );
      }

      /** Get Default monitor */
      auto monitor_nbr = xcb_setup_roots_iterator( xcb_get_setup( p_xcb_connection.get( ) ) ).rem;

      /** Loop through all available monitors. */
      auto iter = xcb_setup_roots_iterator( xcb_get_setup( p_xcb_connection.get( ) ) );
      while ( monitor_nbr-- > 1 )
      {
         xcb_screen_next( &iter ); // TODO: Allow user to pick their prefered monitor.
      }
      p_xcb_screen = iter.data;

      xcb_window = xcb_generate_id( p_xcb_connection.get( ) );

      window_logger->info( "Window ID generated: {0}.", std::to_string( xcb_window ) );

      uint32_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
      uint32_t value_list[32];
      value_list[0] = p_xcb_screen->black_pixel;
      value_list[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
         XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION;

      /*
      xcb_xkb_use_extension( p_xcb_connection_.get(), XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION );

      xcb_xkb_per_client_flags( p_xcb_connection_.get(), XCB_XKB_ID_USE_CORE_KBD,
                                XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT,
                                XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT,
                                0,0,0 );
       */

      if ( is_fullscreen )
      {
         window_logger->info( "Window fullscreen mode." );

         size.x = p_xcb_screen->width_in_pixels;
         size.y = p_xcb_screen->height_in_pixels;
      }

      xcb_create_window( p_xcb_connection.get( ), /* Connection             */
         XCB_COPY_FROM_PARENT,                    /* Depth ( same as root ) */
         xcb_window,                              /* Window ID              */
         p_xcb_screen->root,                      /* Parent Window          */
         position.x, position.y,                  /* Window Position        */
         size.x, size.y, 10,                      /* Window + border Size   */
         XCB_WINDOW_CLASS_INPUT_OUTPUT,           /* Class                  */
         p_xcb_screen->root_visual,               /* Visual                 */
         value_mask, value_list                   /* Masks                  */
      );

      window_logger->info( "Window created." );

      auto p_reply = intern_atom_helper( p_xcb_connection.get( ), true, "WM_PROTOCOLS" );

      p_xcb_wm_delete_window = intern_atom_helper( p_xcb_connection.get( ), false, "WM_DELETE_WINDOW" );

      /** Allows checking of window closing event. */
      xcb_change_property(
         p_xcb_connection.get( ), XCB_PROP_MODE_REPLACE, xcb_window, p_reply->atom, 4, 32, 1, &p_xcb_wm_delete_window->atom );

      /** Change the title of the window. */
      xcb_change_property(
         p_xcb_connection.get( ), XCB_PROP_MODE_REPLACE, xcb_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title.size( ), title.c_str( ) );

      /** Set the window to fullscreen if fullscreen is enabled. */
      if ( is_fullscreen )
      {
         auto p_atom_wm_state = intern_atom_helper( p_xcb_connection.get( ), false, "_NET_WM_STATE" );
         auto p_atom_wm_fullscreen = intern_atom_helper( p_xcb_connection.get( ), false, "_NET_WM_STATE_FULLSCREEN" );

         xcb_change_property( p_xcb_connection.get( ), XCB_PROP_MODE_REPLACE, xcb_window, p_atom_wm_state->atom, XCB_ATOM_ATOM, 32, 1,
            &p_atom_wm_fullscreen->atom );
      }
      xcb_map_window( p_xcb_connection.get( ), xcb_window );
      xcb_flush( p_xcb_connection.get( ) );
#endif
   }
   window::window( window &&wnd ) { *this = std::move( wnd ); }
   window::~window( ) {}

   window &window::operator=( window &&rhs )
   {
      if ( this != &rhs )
      {
         title = std::move( rhs.title );

         position = rhs.position;
         rhs.position = {};

         size = rhs.size;
         rhs.size = {};

         is_wnd_open = rhs.is_wnd_open;
         rhs.is_wnd_open = false;

         is_fullscreen = rhs.is_fullscreen;
         rhs.is_fullscreen = false;

#if defined( VK_USE_PLATFORM_XCB_KHR )
         p_xcb_connection = std::move( rhs.p_xcb_connection );
         p_xcb_screen = std::move( p_xcb_screen );
         xcb_window = std::move( xcb_window );
         p_xcb_wm_delete_window = std::move( rhs.p_xcb_wm_delete_window );

         default_screen_id = rhs.default_screen_id;
         rhs.default_screen_id = -1;
#endif
      }

      return *this;
   }

   bool window::is_open( ) { return is_wnd_open; }

   void window::poll_events( )
   {
#if defined( VK_USE_PLATFORM_XCB_KHR )
      xcb_generic_event_t *e;
      while ( ( e = xcb_poll_for_event( p_xcb_connection.get( ) ) ) )
      {
         switch ( e->response_type & 0x7f )
         {
            case XCB_CLIENT_MESSAGE:
            {
               const auto *message_event = reinterpret_cast<const xcb_client_message_event_t *>( e );

               if ( message_event->data.data32[0] == p_xcb_wm_delete_window->atom )
               {
                  is_wnd_open = false;

                  auto event = window_close_event{};
                  event.is_closed = true;

                  window_close_handler.send_message( event );
               }
            }
            break;
            case XCB_DESTROY_NOTIFY:
            {
               is_wnd_open = false;

               auto event = window_close_event{};
               event.is_closed = true;

               window_close_handler.send_message( event );
            }
            break;
            case XCB_CONFIGURE_NOTIFY:
            {
               const auto *motion_event = reinterpret_cast<const xcb_configure_notify_event_t *>( e );

               position.x = static_cast<uint32_t>( motion_event->x );
               position.y = static_cast<uint32_t>( motion_event->y );

               if ( size.x != static_cast<uint32_t>( motion_event->width ) && size.y != static_cast<uint32_t>( motion_event->height ) )
               {
                  size.x = static_cast<uint32_t>( motion_event->width );
                  size.y = static_cast<uint32_t>( motion_event->height );

                  auto event = framebuffer_resize_event{};
                  event.size = size;

                  framebuffer_resize_handler.send_message( event );
               }
            }
            break;
            case XCB_KEY_PRESS:
            {
               const auto *xcb_key_press = reinterpret_cast<const xcb_key_press_event_t *>( e );

               auto event = key_event{};
               event.code = static_cast<keyboard::key>( xcb_key_press->detail );
               event.state = keyboard::key_state::pressed;

               key_handler.send_message( event );
            }
            break;
            case XCB_KEY_RELEASE:
            {
               const auto *xcb_key_release = reinterpret_cast<const xcb_key_release_event_t *>( e );

               auto event = key_event{};
               event.code = static_cast<keyboard::key>( xcb_key_release->detail );
               event.state = keyboard::key_state::released;

               key_handler.send_message( event );
            }
            break;
            case XCB_BUTTON_PRESS:
            {
               const auto *button_press_event = reinterpret_cast<const xcb_button_press_event_t *>( e );

               auto event = mouse_button_event{};
               event.code = static_cast<mouse::button>( button_press_event->detail );
               event.state = mouse::button_state::pressed;
               event.position = {static_cast<int32_t>( button_press_event->event_x ), static_cast<int32_t>( button_press_event->event_y )};

               mouse_button_handler.send_message( event );
            }
            break;
            case XCB_BUTTON_RELEASE:
            {
               const auto *button_release_event = reinterpret_cast<const xcb_button_release_event_t *>( e );

               auto event = mouse_button_event{};
               event.code = static_cast<mouse::button>( button_release_event->detail );
               event.state = mouse::button_state::released;
               event.position = {
                  static_cast<int32_t>( button_release_event->event_x ), static_cast<int32_t>( button_release_event->event_y )};

               mouse_button_handler.send_message( event );
            }
            break;
            case XCB_MOTION_NOTIFY:
            {
               const auto *cursor_motion = reinterpret_cast<const xcb_motion_notify_event_t *>( e );

               auto event = mouse_motion_event{};
               event.position = {static_cast<int32_t>( cursor_motion->event_x ), static_cast<int32_t>( cursor_motion->event_y )};

               mouse_motion_handler.send_message( event );
            }
            break;
         }

         free( e );
      }
#endif
   }

   std::variant<VkSurfaceKHR, vk::error> window::create_surface( vk::instance_t instance ) const
   {
#if defined( VK_USE_PLATFORM_XCB_KHR )
      VkSurfaceKHR surface = VK_NULL_HANDLE;

      VkXcbSurfaceCreateInfoKHR create_info{};
      create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
      create_info.connection = p_xcb_connection.get( );
      create_info.window = xcb_window;

      vk::error const err( vk::result_t( vkCreateXcbSurfaceKHR( instance.value( ), &create_info, nullptr, &surface ) ) );

      if ( err.is_error( ) )
      {
         return err;
      }
      else
      {
         return surface;
      }
#endif
   }

   glm::uvec2 window::get_size( ) const { return size; }
} // Namespace ui
