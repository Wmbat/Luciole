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

#include <stdexcept>
#include <iostream>
#include <array>
#include <set>

#include "console.h"
#include "vulkan_core.h"

namespace TWE
{
    VKAPI_ATTR VkBool32 VKAPI_CALL
    debug_callback_function( VkDebugReportFlagsEXT flags,
                             VkDebugReportObjectTypeEXT objType,
                             uint64_t obj, size_t location,
                             int32_t code, const char* layerPrefix,
                             const char* msg, void* userData )
    {
        std::cerr << "validation layer: " << msg << std::endl;

        return VK_FALSE;
    }

    vulkan_core::vulkan_core( const window& wnd, const std::string& app_name, uint32_t app_version )
    {
        const std::vector<const char*> instance_extensions
        {
            VK_KHR_SURFACE_EXTENSION_NAME,
#ifndef NDEBUG
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif

#if defined( _WIN32 )
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
            VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
#elif defined( VK_USE_PLATFORM_XCB_KHR )
            VK_KHR_XCB_SURFACE_EXTENSION_NAME
#endif
        };

        const std::vector<const char*> debug_layers = { "VK_LAYER_LUNARG_standard_validation" };

        if( !check_instance_extension_support( instance_extensions ) )
        {
            throw std::runtime_error{ "Extensions requested, but not supported!" };
        }

        if( !enable_debug_layers && !check_debug_layer_support( debug_layers ) )
        {
            throw std::runtime_error{ "VK_LAYER_LUNARG_standar_validation not supported!" };
        }

        instance_ = check_vk_return_state(
                create_instance( instance_extensions, debug_layers, app_name, app_version ),
                "Failed to create Instance!" );
        console::log( "Vulkan Instance created.\n" );


        if constexpr( enable_debug_layers )
        {
            debug_report_ = check_vk_return_state( create_debug_report( instance_ ), "Failed to create Debug Report Callback!" );
            console::log( "Vulkan Debug Report Callback created.\n" );
        }

        surface_ = check_vk_return_state( create_surface( wnd, instance_ ), "Failed to create Surface!" );
        console::log( "Vulkan Surface created.\n" );

        console::flush();
    }
    vulkan_core::vulkan_core( vulkan_core&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    vulkan_core::~vulkan_core( )
    {
        vkDestroySurfaceKHR( instance_, surface_, nullptr );


        if constexpr( enable_debug_layers )
        {
            vkDestroyDebugReportCallbackEXT( instance_, debug_report_, nullptr );
        }

        vkDestroyInstance( instance_, nullptr );
    }

    vulkan_core& vulkan_core::operator=( vulkan_core &&rhs ) noexcept
    {
        if( this != &rhs )
        {
            instance_ = rhs.instance_;
            rhs.instance_ = VK_NULL_HANDLE;

            if constexpr( enable_debug_layers )
            {
                debug_report_ = rhs.debug_report_;
                rhs.debug_report_ = VK_NULL_HANDLE;
            }

            surface_ = rhs.surface_;
            rhs.surface_ = VK_NULL_HANDLE;
        }

        return *this;
    }

    const VkInstance& vulkan_core::get_instance( ) const noexcept
    {
        return instance_;
    }
    const VkSurfaceKHR& vulkan_core::get_surface( ) const noexcept
    {
        return surface_;
    }
    const VkPhysicalDevice& vulkan_core::get_physical_device( ) const noexcept
    {
        return physical_device_;
    }
    const VkDevice& vulkan_core::get_device( ) const noexcept
    {
        return device_;
    }

    const vk_return_obj<VkInstance> vulkan_core::create_instance( const std::vector<const char*>& extensions,
                                                                  const std::vector<const char*>& debug_layers ,
                                                                  const std::string& app_name, uint32_t app_version ) const noexcept
    {
        VkInstance instance = VK_NULL_HANDLE;

        const VkApplicationInfo app_info =
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = app_name.c_str( ),
            .applicationVersion = app_version,
            .pEngineName = "The Wonbat Engine",
            .engineVersion = VK_MAKE_VERSION( 0, 0, 0 ),
            .apiVersion = VK_API_VERSION_1_1,
        };

        if( enable_debug_layers )
        {
            const VkInstanceCreateInfo create_info =
            {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = { },
                .pApplicationInfo = &app_info,
                .enabledLayerCount = static_cast<uint32_t>( debug_layers.size() ),
                .ppEnabledLayerNames = debug_layers.data(),
                .enabledExtensionCount = static_cast<uint32_t>( extensions.size() ),
                .ppEnabledExtensionNames = extensions.data(),
            };

            return { vkCreateInstance( &create_info, nullptr, &instance ), instance };
        }
        else
        {
            const VkInstanceCreateInfo create_info =
            {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = { },
                .pApplicationInfo = &app_info,
                .enabledLayerCount = 0,
                .ppEnabledLayerNames = nullptr,
                .enabledExtensionCount = static_cast<uint32_t>( extensions.size() ),
                .ppEnabledExtensionNames = extensions.data(),
            };

            return { vkCreateInstance( &create_info, nullptr, &instance ), instance };
        }
    }

    const vk_return_obj<VkDebugReportCallbackEXT> vulkan_core::create_debug_report( const VkInstance& instance ) const noexcept
    {
        VkDebugReportCallbackEXT debug_report;

        const VkDebugReportCallbackCreateInfoEXT create_info
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = { },
            .pfnCallback = debug_callback_function,
            .pUserData = nullptr
        };

        return { vkCreateDebugReportCallbackEXT( instance, &create_info, nullptr, &debug_report ), debug_report };
    }

    const vk_return_obj<VkSurfaceKHR> vulkan_core::create_surface( const window& wnd, const VkInstance& instance ) const noexcept
    {
        return wnd.create_surface( instance );
    }

    const vk_return_obj<VkPhysicalDevice> vulkan_core::pick_physical_device( const VkSurfaceKHR& surface,
            const std::vector<const char*>& device_extensions ) const noexcept
    {
        uint32_t num_devices = 0;
        vkEnumeratePhysicalDevices( instance_, &num_devices, nullptr );

        std::vector<VkPhysicalDevice> available_devices( num_devices );
        vkEnumeratePhysicalDevices( instance_, &num_devices, available_devices.data() );

        for( auto& physical_device : available_devices )
        {
            if( is_physical_device_suitable( surface, physical_device, device_extensions ) )
            {
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties( physical_device, &properties );

                auto i = 0;
                std::cout << "(" << i << ") " << properties.deviceName << ": " << properties.deviceType << "\n";

                ++i;
            }
        }
    }

    const vk_return_obj<VkDevice> vulkan_core::create_device( ) const noexcept
    {

    }

    bool vulkan_core::check_instance_extension_support( const std::vector<const char*>& instance_extensions ) const noexcept
    {
        uint32_t count;
        vkEnumerateInstanceExtensionProperties( nullptr, &count, nullptr );

        std::vector<VkExtensionProperties> available_extensions( count );
        vkEnumerateInstanceExtensionProperties( nullptr, &count, available_extensions.data() );

        for( const auto& extension : instance_extensions )
        {
            bool is_supported = false;

            for( const auto& extension_property : available_extensions )
            {
                if( strcmp( extension, extension_property.extensionName ) == 0 )
                {
                    is_supported = true;
                    break;
                }
            }

            if( !is_supported )
            {
                return false;
            }
        }

        return true;
    }

    bool vulkan_core::check_debug_layer_support( const std::vector<const char *> &debug_layers ) const noexcept
    {
        uint32_t count;
        vkEnumerateInstanceLayerProperties( &count, nullptr );

        std::vector<VkLayerProperties> available_layers( count );
        vkEnumerateInstanceLayerProperties( &count, available_layers.data() );

        for( const auto& layer : debug_layers )
        {
            bool is_supported = false;

            for( const auto& layer_properties : available_layers )
            {
                if( strcmp( layer, layer_properties.layerName ) == 0 )
                {
                    is_supported = true;
                    break;
                }
            }

            if( !is_supported )
            {
                return false;
            }
        }

        return true;
    }

    bool vulkan_core::is_physical_device_suitable( const VkSurfaceKHR& surface, const VkPhysicalDevice& physical_device,
                                                   const std::vector<const char*>& device_extensions ) const noexcept
    {
        auto indices = find_queue_family_indices( surface, physical_device );

        return indices.is_complete() &&
               check_physical_device_extension_support( physical_device, device_extensions ) &&
               is_swapchain_adequate( surface, physical_device );
    }

    bool vulkan_core::check_physical_device_extension_support( const VkPhysicalDevice &physical_device,
                                                               const std::vector<const char *> &device_extensions ) const noexcept
    {
        std::set<std::string> required_extensions( device_extensions.cbegin(), device_extensions.cend() );

        uint32_t count;
        vkEnumerateDeviceExtensionProperties( physical_device, nullptr, &count, nullptr );

        std::vector<VkExtensionProperties> properties( count );
        vkEnumerateDeviceExtensionProperties( physical_device, nullptr, &count, properties.data() );

        for( const auto& property : properties )
        {
            required_extensions.erase( property.extensionName );
        }

        return required_extensions.empty();
    }

    bool vulkan_core::is_swapchain_adequate( const VkSurfaceKHR& surface, const VkPhysicalDevice& physical_device ) const noexcept
    {
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR( physical_device, surface, &format_count, nullptr );

        std::vector<VkSurfaceFormatKHR> surface_formats( format_count );
        vkGetPhysicalDeviceSurfaceFormatsKHR( physical_device, surface, &format_count, surface_formats.data() );


        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR( physical_device, surface, &present_mode_count, nullptr );

        std::vector<VkPresentModeKHR> present_modes( present_mode_count );
        vkGetPhysicalDeviceSurfacePresentModesKHR( physical_device, surface, &present_mode_count, present_modes.data() );

        return !surface_formats.empty() && !present_modes.empty();
    }


    const vulkan_core::queue_family_indices vulkan_core::find_queue_family_indices(
            const VkSurfaceKHR &surface,
            const VkPhysicalDevice &physical_device ) const noexcept
    {
        queue_family_indices indices;

        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &count, nullptr );

        std::vector<VkQueueFamilyProperties> queue_properties( count );
        vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &count, queue_properties.data() );

        int i = 0;
        for( const auto& queue_property : queue_properties )
        {
            if( queue_property.queueCount > 0 )
            {
                if ( queue_property.queueFlags == VK_QUEUE_GRAPHICS_BIT )
                {
                    indices.graphic_family_ = i;
                }
                if ( queue_property.queueFlags == VK_QUEUE_COMPUTE_BIT )
                {
                    indices.compute_family_ = i;
                }

                VkBool32 present_support;
                vkGetPhysicalDeviceSurfaceSupportKHR( physical_device, i, surface, &present_support );

                if ( present_support )
                {
                    indices.present_family_ = i;
                }
            }

            ++i;
        }

        return indices;
    }
}