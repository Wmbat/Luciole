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

    void context::create_device( ) noexcept
    {
        std::uint32_t properties_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( gpu_, &properties_count, nullptr );
        VkQueueFamilyProperties* queue_family_properties = reinterpret_cast<VkQueueFamilyProperties*>( alloca( sizeof( VkQueueFamilyProperties ) * properties_count ) );
        vkGetPhysicalDeviceQueueFamilyProperties( gpu_, &properties_count, queue_family_properties );

        std::vector<core::context::queue_info> queue_info;
        queue_info.reserve( properties_count );
        for( size_t i = 0; i < properties_count; ++i )
        {
            if ( queue_family_properties[i].queueCount > 0 )
            {
                queue_info.emplace_back( 
                    core::context::queue_info
                    { 
                        .flags_ = queue_family_properties[i].queueFlags,
                        .count_ = queue_family_properties[i].queueCount,
                        .index_ = i 
                    } );
            }
        }

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::vector<std::vector<float>> priorities( queue_info.size( ) );

        for ( size_t i = 0; i < queue_info.size( ); ++i )
        {
            std::vector<float> queue_prorities( queue_info[i].count_ );
            for( auto& priority : queue_prorities )
            {
                priority = 1.0f;
            }
            priorities[i] = queue_prorities;


            const VkDeviceQueueCreateInfo queue_create_info
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = queue_info[i].index_,
                .queueCount = queue_info[i].count_,
                .pQueuePriorities = priorities[i].data( )
            };

            queue_create_infos.emplace_back( queue_create_info );
        }

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures( gpu_, &features );

        std::uint32_t extension_count = 0;
        vkEnumerateDeviceExtensionProperties( gpu_, nullptr, &extension_count, nullptr );
        VkExtensionProperties* extensions = reinterpret_cast<VkExtensionProperties*>( alloca( sizeof( VkExtensionProperties ) * extension_count ) );
        vkEnumerateDeviceExtensionProperties( gpu_, nullptr, &extension_count, extensions );

        for( size_t i = 0; i < extension_count; ++i )
        {
            device_extensions_.emplace_back( extensions[i].extensionName );
        }

        const VkDeviceCreateInfo create_info
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = static_cast<std::uint32_t>( queue_create_infos.size( ) ),
            .pQueueCreateInfos = queue_create_infos.data( ),
            .enabledExtensionCount = static_cast<std::uint32_t>( device_extensions_.size( ) ),
            .ppEnabledExtensionNames = device_extensions_.data( ),
            .pEnabledFeatures = &features
        };

        if ( vkCreateDevice( gpu_, &create_info, nullptr, &device_ ) != VK_NULL_HANDLE )
        {
            core_error( "Failed to create Device." );
        }

        bool has_compute_only = false;
        bool has_transfer_only = false;
        for( const auto& info : queue_info )
        {
            if ( info.is_compute_only( ) )
            {
                VkQueue compute_queue = VK_NULL_HANDLE;
                vkGetDeviceQueue( device_, info.index_, 0, &compute_queue );

                struct queue queue
                {
                    .flags_ = VK_QUEUE_COMPUTE_BIT,
                    .handle_ = compute_queue,
                    .index_ = 0,
                    .family_ = info.index_
                };

                queues_.emplace_back( queue );

                has_compute_only = true;
            }
            if ( info.is_transfer_only( ) )
            {
                VkQueue transfer_queue = VK_NULL_HANDLE;
                vkGetDeviceQueue( device_, info.index_, 0, &transfer_queue );

                struct queue queue
                {
                    .flags_ = VK_QUEUE_TRANSFER_BIT,
                    .handle_ = transfer_queue,
                    .index_ = 0,
                    .family_ = info.index_
                };

                queues_.emplace_back( queue );

                has_transfer_only = true;
            }
        }

        for( const auto& info : queue_info )
        {
            if( info.is_general_purpose( ) )
            {
                VkQueue graphics_queue = VK_NULL_HANDLE;
                vkGetDeviceQueue( device_, info.index_, 0, &graphics_queue );

                {
                    struct queue queue
                    {
                        .flags_ = VK_QUEUE_GRAPHICS_BIT,
                        .handle_ = graphics_queue,
                        .index_ = 0,
                        .family_ = info.index_
                    };

                    queues_.emplace_back( queue );
                }

                if ( info.count_ > 1 )
                {
                    if ( !has_compute_only )
                    {
                        VkQueue compute_queue = VK_NULL_HANDLE;
                        vkGetDeviceQueue( device_, info.index_, 0, &compute_queue );

                        struct queue queue
                        {
                            .flags_ = VK_QUEUE_COMPUTE_BIT,
                            .handle_ = compute_queue,
                            .index_ = 1,
                            .family_ = info.index_
                        };

                        queues_.emplace_back( queue );
                    }
                }

                if ( info.count_ > 2 )
                {
                    if ( !has_transfer_only )
                    {
                        VkQueue transfer_queue = VK_NULL_HANDLE;
                        vkGetDeviceQueue( device_, info.index_, 0, &transfer_queue );

                        struct queue queue
                        {
                            .flags_ = VK_QUEUE_TRANSFER_BIT,
                            .handle_ = transfer_queue,
                            .index_ = 2,
                            .family_ = info.index_
                        };

                        queues_.emplace_back( queue );
                    }
                }
            }
        }

        queues_.shrink_to_fit( ); 
    }
}