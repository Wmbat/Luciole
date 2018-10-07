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
    static void key_callback( GLFWwindow* p_wnd, int key, int scan_code, int action, int mods )
    {
        auto* p_keyboard = reinterpret_cast<keyboard*>( glfwGetWindowUserPointer( p_wnd ) );

        keyboard::key_event e;
        e.id_ = key;
        e.type_ = ( action == GLFW_PRESS || action == GLFW_REPEAT ) ? keyboard::type::pressed : keyboard::type::released;

        p_keyboard->push_key_event( e );
    }

    window::window( uint32_t width, uint32_t height, const std::string& title )
        :
        width_( width ),
        height_( height ),
        title_( title ),
        p_keyboard_( new keyboard( ) )
    {
        if ( !glfwInit( ) )
        {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
        }

        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
        glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );       // temp.
        glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );

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

        glfwSetWindowUserPointer( p_glfw_window_, p_keyboard_ );
    }
    window::window( window&& other ) noexcept
    {

    }
    window::~window( )
    {
        delete p_keyboard_;

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

    void window::close( )
    {

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