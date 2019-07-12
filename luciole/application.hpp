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

#ifndef LUCIOLE_APPLICATION_HPP
#define LUCIOLE_APPLICATION_HPP

#include <memory>

#include <vulkan/vulkan.h>

#include "extension.hpp"
 
#include "window/window.hpp"

#if defined( NDEBUG )
    static constexpr bool enable_debug_layers = false;
#else
    static constexpr bool enable_debug_layers = true;
#endif

class application
{
public:
    application( );
    ~application( );

    void run( );

private:
    void create_instance( const VkApplicationInfo& app_info );
    void destroy_instance( );

    void create_debug_messenger( );
    void destroy_debug_messenger( );

    void create_surface( );
    void destroy_surface( );

    void pick_gpu( );
    int rate_gpu( const VkPhysicalDevice gpu );

    void create_device( );
    void destroy_device( );

private:
    std::unique_ptr<window> p_wnd_;

    VkInstance instance_ = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    VkPhysicalDevice gpu_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;
    
    VkQueue graphics_queue_ = VK_NULL_HANDLE;
    VkQueue transfer_queue_ = VK_NULL_HANDLE;
    VkQueue compute_queue_ = VK_NULL_HANDLE;

    const std::vector<const char*> validation_layers_ = { "VK_LAYER_KHRONOS_validation" };

    std::vector<extension> instance_extensions_ = 
    {
#if defined( VK_USE_PLATFORM_WIN32_KHR )
        extension{ .priority_ = extension::priority::e_required, .found_ = false, .name_ = "VK_KHR_win32_surface" },
#elif defined( VK_USE_PLATFORM_XCB_KHR )
        extension{ .priority_ = extension::priority::e_required, .found_ = false, .name_ = "VK_KHR_xcb_surface" },
#endif
        extension{ .priority_ = extension::priority::e_required, .found_ = false, .name_ = "VK_KHR_surface" },
        extension{ .priority_ = extension::priority::e_optional, .found_ = false, .name_ = "VK_KHR_get_surface_capabilities2" },
        extension{ .priority_ = extension::priority::e_optional, .found_ = false, .name_ = "VK_EXT_debug_utils" }
    };

    std::vector<extension> device_extensions_ =
    {
        extension{ .priority_ = extension::priority::e_required, .found_ = false, .name_ = "VK_KHR_swapchain" }
    };
};

#endif // LUCIOLE_APPLICATION_HPP