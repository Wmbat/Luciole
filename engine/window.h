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

#ifndef VULKAN_PROJECT_WINDOW_H
#define VULKAN_PROJECT_WINDOW_H

#if NDEBUG
static constexpr bool enable_validation_layers = false;
#else
static constexpr bool enable_validation_layers = true;
#endif

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

namespace engine
{
    class window
    {
    public:
        window( ) = default;
        window( uint32_t width, uint32_t height, const std::string& title );
        window( const window& other ) = delete;
        window( window&& other ) noexcept;
        ~window( );

        bool is_open( );

        void poll_events( );

        std::vector<const char*> get_required_extensions( ) const noexcept;
        std::pair<VkBool32, VkSurfaceKHR> create_surface( const VkInstance& instance ) const noexcept;

        const uint32_t get_width( ) const noexcept;
        const uint32_t get_height( ) const noexcept;

        window& operator=( const window& other ) = delete;
        window& operator=( window&& other ) noexcept;

    private:
        GLFWwindow* p_glfw_window_ = nullptr;

        std::string title_;
        uint32_t width_ = 0;
        uint32_t height_ = 0;
    };
}

#endif //VULKAN_PROJECT_WINDOW_H
