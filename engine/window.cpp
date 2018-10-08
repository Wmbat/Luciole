/*!
 *  Copyright (C) 2018 BouwnLaw
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

#include <iostream>

#include "window.h"

namespace engine
{
    static void key_callback( GLFWwindow *p_wnd, int key, int scan_code, int action, int mods )
    {
        auto *p_input_device = reinterpret_cast<window::input_devices *>( glfwGetWindowUserPointer( p_wnd ));

        keyboard::key_event e;
        e.id_ = key;
        e.type_ = ( action == GLFW_PRESS || action == GLFW_REPEAT ) ? keyboard::type::pressed
                                                                    : keyboard::type::released;

        p_input_device->keyboard_.push_key_event( e );
    }

    static void cursor_position_callback( GLFWwindow *p_wnd, double x_pos, double y_pos )
    {
        auto *p_input_device = reinterpret_cast<window::input_devices *>( glfwGetWindowUserPointer( p_wnd ));

        mouse::cursor_event e;
        e.x_pos_ = x_pos;
        e.y_pos_ = y_pos;

        p_input_device->mouse_.push_cursor_event( e );
    }

    static void mouse_button_callback( GLFWwindow *p_wnd, int button, int action, int mods )
    {
        auto *p_input_device = reinterpret_cast<window::input_devices *>( glfwGetWindowUserPointer( p_wnd ));

        mouse::button_event e;
        e.button_id_ = button;
        e.type_ = ( action == GLFW_PRESS || action == GLFW_REPEAT ) ? mouse::type::pressed : mouse::type::released;

        p_input_device->mouse_.push_button_event( e );
    }

    static void window_position_callback( GLFWwindow *p_wnd, int x, int y )
    {

    }

    static void window_size_callback( GLFWwindow *p_wnd, int width, int height )
    {

    }

    static void framebuffer_size_callback( GLFWwindow* p_wnd, int x, int y )
    {

    }

    window::window( uint32_t width, uint32_t height, const std::string& title )
        :
        width_( width ),
        height_( height ),
        title_( title )
    {
        if ( !glfwInit( ) )
        {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
        }

        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
        glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );       // temp.
        glfwWindowHint( GLFW_DECORATED, GLFW_TRUE );

        p_glfw_window_ = glfwCreateWindow( width_, height_, title_.c_str( ), nullptr, nullptr );

        if ( !p_glfw_window_ )
        {
            glfwDestroyWindow( p_glfw_window_ );
            glfwTerminate( );

            std::cerr << "Failed to create GLFW window!" << std::endl;
        }

        glfwSetWindowPos( p_glfw_window_, 100, 100 );

        /// callbacks ///
        glfwSetKeyCallback( p_glfw_window_, key_callback );
        glfwSetCursorPosCallback( p_glfw_window_, cursor_position_callback );
        glfwSetMouseButtonCallback( p_glfw_window_, mouse_button_callback );

        glfwSetWindowPosCallback( p_glfw_window_, window_position_callback );
        glfwSetWindowSizeCallback( p_glfw_window_, window_size_callback );
        glfwSetFramebufferSizeCallback( p_glfw_window_, framebuffer_size_callback );


        glfwSetWindowUserPointer( p_glfw_window_, &input_devices_ );
    }
    window::window( window&& other ) noexcept
    {

    }
    window::~window( )
    {
        if( p_glfw_window_ != nullptr )
        {
            glfwDestroyWindow( p_glfw_window_ );
        }

        glfwTerminate( );
    }

    bool window::is_open( )
    {
        return !glfwWindowShouldClose( p_glfw_window_ );
    }

    void window::poll_events( )
    {
        glfwPollEvents( );
    }

    std::vector<const char*> window::get_required_extensions( ) const noexcept
    {
        uint32_t extension_count;
        const char** extensions = glfwGetRequiredInstanceExtensions( &extension_count );

        std::vector<const char*> vulkan_extensions( extensions, extensions + extension_count );

        if( enable_validation_layers )
            vulkan_extensions.emplace_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );

        return vulkan_extensions;
    }
    window::surface window::create_surface( const VkInstance& instance ) const noexcept
    {
        VkSurfaceKHR surface_handle;

        return { glfwCreateWindowSurface( instance, p_glfw_window_, nullptr, &surface_handle ), surface_handle };
    }

    const uint32_t window::get_width( ) const noexcept
    {
        return width_;
    }
    const uint32_t window::get_height( ) const noexcept
    {
        return height_;
    }

    window& window::operator=( window&& other ) noexcept
    {
        if( this != &other )
        {
            if( p_glfw_window_ != nullptr )
            {
                glfwDestroyWindow( p_glfw_window_ );
            }

            p_glfw_window_ = other.p_glfw_window_;
            other.p_glfw_window_ = nullptr;

            title_ = other.title_;
            other.title_ = "";

            width_ = other.width_;
            other.width_ = 0;

            height_ = other.height_;
            other.height_ = 0;
        }

        return *this;
    }
}