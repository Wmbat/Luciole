/*
 * @author wmbat@protonmail.com
 *
 * Copyright (C) 2018-2019 Wmbat
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * You should have received a copy of the GNU General Public License
 * GNU General Public License for more details.
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <map>

#include "context.hpp"

#include "../utilities/log.hpp"

namespace lcl::gfx
{ 
    context::context( base_window* p_wnd, const std::string& app_name, std::uint32_t app_version )
    {
        std::uint32_t api_version;
        vkEnumerateInstanceVersion( &api_version );

        core_info( "Using Vulkan {}.{}.{}", VK_VERSION_MAJOR( api_version ), VK_VERSION_MINOR( api_version ), VK_VERSION_PATCH( api_version ) );

        const VkApplicationInfo app_info 
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = nullptr, 
            .applicationVersion = 0,
            .pEngineName = "Luciole",
            .engineVersion = VK_MAKE_VERSION( 0, 0, 6 ),
            .apiVersion = api_version
        };

        create_instance( app_info );
        create_debug_messenger( );
        create_surface( p_wnd );

        pick_gpu( );

        create_device( );
    }

    context::~context( )
    {
        destroy_device( );
        destroy_surface( );
        destroy_debug_messenger( );
        destroy_instance( );
    }

    void context::create_surface( base_window* p_wnd ) noexcept
    {
        surface_ = p_wnd->create_surface( instance_ );
    }

    void context::destroy_surface( ) noexcept
    {
        if ( surface_ != VK_NULL_HANDLE )
        {
            vkDestroySurfaceKHR( instance_, surface_, nullptr );
            surface_ = VK_NULL_HANDLE;
        }
    }

    void context::pick_gpu( ) noexcept
    {
        std::uint32_t physical_device_count = 0;
        vkEnumeratePhysicalDevices( instance_, &physical_device_count, nullptr );
        VkPhysicalDevice* physical_devices = reinterpret_cast<VkPhysicalDevice*>( alloca( sizeof( VkPhysicalDevice ) * physical_device_count ) );
        vkEnumeratePhysicalDevices( instance_, &physical_device_count, physical_devices );

        std::multimap<std::uint32_t, VkPhysicalDevice> candidates;

        for( size_t i = 0; i < physical_device_count; ++i )
        {
            std::uint32_t rating = rate_gpu( physical_devices[i] );
            candidates.insert( { rating, physical_devices[i] } );
        }

        if ( candidates.rbegin( )->first > 0 )
        {
            gpu_ = candidates.begin( )->second;

            VkPhysicalDeviceProperties2 properties
            {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
                .pNext = nullptr
            };

            vkGetPhysicalDeviceProperties2( gpu_, &properties );

            core_info( "GPU: {}", properties.properties.deviceName );
        }
        else
        {
            core_error( "Failed to find a suitable GPU" );
        }
    }

    int context::rate_gpu( const VkPhysicalDevice gpu ) noexcept
    {
        std::uint32_t properties_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( gpu, &properties_count, nullptr );
        VkQueueFamilyProperties* queue_family_properties = reinterpret_cast<VkQueueFamilyProperties*>( alloca( sizeof( VkQueueFamilyProperties ) * properties_count ) );
        vkGetPhysicalDeviceQueueFamilyProperties( gpu, &properties_count, queue_family_properties );

        bool is_rendering_capable = false;
        for( size_t i = 0; i < properties_count; ++i )
        {
            if ( queue_family_properties[i].queueCount > 0 )
            {
                VkBool32 surface_support = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR( gpu, i, surface_, &surface_support );

                if ( ( queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ) && ( surface_support == VK_TRUE ) )
                {
                    is_rendering_capable = true;
                }
            }
        }

        if ( !is_rendering_capable )
            return 0;

        std::uint32_t surface_format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR( gpu, surface_, &surface_format_count, nullptr );

        if ( surface_format_count == 0 )
            return 0;

        std::uint32_t present_mode_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR( gpu, surface_, &present_mode_count, nullptr );

        if ( present_mode_count == 0 )
            return 0;

        std::uint32_t score = 0;

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties( gpu, &properties );

        if ( properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
        {
            score += 2;
        }
        else if ( properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU )
        {
            score += 1;
        }

        return score;
    }
}