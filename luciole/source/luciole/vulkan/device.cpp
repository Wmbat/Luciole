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

#include "device.hpp"

namespace lcl::vulkan
{
    device::device( const instance& instance, const surface& surface )
        :
        instance_( instance.handle_ ),
        surface_( surface.handle_ )
    {
        std::multimap<uint32_t, VkPhysicalDevice> candidates;
        
        std::uint32_t gpu_count = 0;
        vkEnumeratePhysicalDevices( instance.handle_, &gpu_count, nullptr );
        std::vector<VkPhysicalDevice> physical_devices( gpu_count );
        vkEnumeratePhysicalDevices( instance.handle_, &gpu_count, physical_devices.data( ) );

        for ( const auto& gpu : physical_devices )
        {
            if ( is_gpu_suitable( gpu, surface ) )
            {
                uint32_t score = 0;
                
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
                else
                {
                    score += 0;
                }

                candidates.insert( { score, gpu } );
            }
        }
        
        if ( candidates.size( ) == 0 )
        {
            core_error( "Failed to find a suitable GPU with the required/demanded specifications" );
        }

        gpu_ = candidates.begin()->second;

        std::uint32_t extension_count = 0;
        vkEnumerateDeviceExtensionProperties( gpu_, nullptr, &extension_count, nullptr );
        std::vector<VkExtensionProperties> device_extensions( extension_count );
        vkEnumerateDeviceExtensionProperties( gpu_, nullptr, &extension_count, device_extensions.data( ) );

        for ( const auto& extension_property : device_extensions )
        {
            for( const auto& extension : extensions_.extensions_ )
            {
                if ( strcmp( extension_property.extensionName, extension.name_ ) == 0 )
                {
                    extensions_.enable( extension_property.extensionName );
                }
            }
        }

        const auto enabled_extensions = extensions_.get_enabled_extension_names( );

        for( const char* extension : enabled_extensions )
        {
            core_info( "Device extension \"{}\" enabled.", std::string{ extension } );
        }

        //////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////

        const auto queue_family_infos = get_queue_family_infos( gpu_ );
        
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

        for( size_t i = 0; i < queue_family_infos.size( ); ++i )
        {
            VkDeviceQueueCreateInfo create_info = { };
            create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            create_info.queueCount = queue_family_infos[i].queue_count_;

            if ( queue_family_infos[i].is_dedicated_transfer( ) )
            {
                create_info.queueFamilyIndex = queue_family_infos[i].transfer_.value( );
            }
            else if ( queue_family_infos[i].is_general_purpose( ) )
            {
                create_info.queueFamilyIndex = queue_family_infos[i].graphics_.value( );
            }

            queue_create_infos.push_back( create_info );
        }

        VkPhysicalDeviceFeatures features = { };
        vkGetPhysicalDeviceFeatures( gpu_, &features );

        VkDeviceCreateInfo device_create_info = { };
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.queueCreateInfoCount = static_cast<std::uint32_t>( queue_create_infos.size( ) );
        device_create_info.pQueueCreateInfos = queue_create_infos.data( );
        device_create_info.enabledExtensionCount = static_cast<std::uint32_t>( enabled_extensions.size( ) );
        device_create_info.ppEnabledExtensionNames = enabled_extensions.data( );
        device_create_info.pEnabledFeatures = &features;
    
        if ( vkCreateDevice( gpu_, &device_create_info, nullptr, &handle_ ) != VK_SUCCESS )
        {
            core_error( "Failed to create logical device."  );
        }

        bool has_dedicated_transfer = false;
        for( const auto& info : queue_family_infos )
        {
            if ( info.is_dedicated_transfer( ) )
            {
                has_dedicated_transfer = true;

                VkQueue handle = VK_NULL_HANDLE;
                vkGetDeviceQueue( handle_, info.transfer_.value( ), 0, &handle );

                queues_.emplace_back( queue{ queue::type::e_transfer, info.transfer_.value( ), 0, handle } );
            }
        }

        for( const auto& info : queue_family_infos )
        {
            if ( info.is_general_purpose( ) )
            {
                VkQueue graphics_handle = VK_NULL_HANDLE;
                vkGetDeviceQueue( handle_, info.graphics_.value( ), 0, &graphics_handle );

                queues_.emplace_back( queue{ queue::type::e_graphics, info.graphics_.value( ), 0, graphics_handle } );
            
                if ( !has_dedicated_transfer )
                {
                    VkQueue transfer_handle = VK_NULL_HANDLE;

                    if ( info.queue_count_ > 1 )
                    {
                        vkGetDeviceQueue( handle_, info.transfer_.value( ), 1, &transfer_handle );

                        queues_.emplace_back( queue{ queue::type::e_transfer, info.transfer_.value( ), 1, transfer_handle } );    
                    }
                    else
                    {
                        vkGetDeviceQueue( handle_, info.transfer_.value( ), 0, &transfer_handle );
                    
                        queues_.emplace_back( queue{ queue::type::e_transfer, info.transfer_.value( ), 0, transfer_handle } );
                    }
                }
            }
        }
    }
    device::device( device&& other )
    {
        *this = std::move( other );
    }
    device::~device( )
    {
        if ( handle_ != VK_NULL_HANDLE )
            vkDestroyDevice( handle_, nullptr );
    }

    device& device::operator=( device&& rhs )
    {
        if ( this != &rhs )
        {
            gpu_ = rhs.gpu_;
            rhs.gpu_ = VK_NULL_HANDLE;
        
            handle_ = rhs.handle_;
            rhs.handle_ = VK_NULL_HANDLE;

            queues_ = std::move( rhs.queues_ );

            instance_ = rhs.instance_;
            rhs.instance_ = VK_NULL_HANDLE;

            surface_ = rhs.surface_;
            rhs.surface_ = VK_NULL_HANDLE;

            extensions_ = rhs.extensions_;
        }
        
        return *this;
    }

    std::vector<command_pool> device::get_command_pools( ) const
    {
        std::vector<command_pool> pools;

        for( const auto& queue : queues_ )
        {
            VkCommandPoolCreateInfo create_info = { };
            create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            create_info.queueFamilyIndex = queue.family_index_;

            VkCommandPool command_pool = VK_NULL_HANDLE;
            vkCreateCommandPool( handle_, &create_info, nullptr, &command_pool );
        
            if ( queue.type_ == queue::type::e_graphics )
                pools.emplace_back( vulkan::command_pool{ command_pool::type::e_graphics, command_pool } );
            else if ( queue.type_ == queue::type::e_transfer )
                pools.emplace_back( vulkan::command_pool{ command_pool::type::e_transfer, command_pool } );
        }

        return pools;
    }

    bool device::is_gpu_suitable( VkPhysicalDevice gpu, const surface& surface ) const
    {
        device_extensions curr_extensions;

        std::uint32_t extension_count = 0;
        vkEnumerateDeviceExtensionProperties( gpu, nullptr, &extension_count, nullptr );
        std::vector<VkExtensionProperties> device_extensions( extension_count );
        vkEnumerateDeviceExtensionProperties( gpu, nullptr, &extension_count, device_extensions.data( ) );

        // Enable all extensions supported by the GPU.
        for ( const auto& extension_property : device_extensions )
        {
            for( const auto& extension : curr_extensions.extensions_ )
            {
                if ( strcmp( extension_property.extensionName, extension.name_ ) == 0 )
                {
                    curr_extensions.enable( extension_property.extensionName );
                }
            }
        }

        if ( !curr_extensions.is_core_enabled( ) )
        {
            return false;
        }

        bool is_rendering_capable = false;

        std::uint32_t property_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( gpu, &property_count, nullptr );
        VkQueueFamilyProperties* p_properties = reinterpret_cast<VkQueueFamilyProperties*>( alloca( sizeof( VkQueueFamilyProperties ) * property_count ) );
        vkGetPhysicalDeviceQueueFamilyProperties( gpu, &property_count, p_properties );

        for( size_t i = 0; i < property_count; ++i )
        {
            if ( p_properties[i].queueCount > 0 )
            {
                VkBool32 is_surface_supported = false;
                vkGetPhysicalDeviceSurfaceSupportKHR( gpu, i, surface.handle_, &is_surface_supported );

                if ( p_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && is_surface_supported )
                {
                    is_rendering_capable = true;
                }
            }
        }
        
        std::uint32_t surface_format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR( gpu, surface.handle_, &surface_format_count, nullptr );

        std::uint32_t present_mode_count = 0;
        vkGetPhysicalDevicePresentRectanglesKHR( gpu, surface.handle_, &present_mode_count, nullptr );

        return is_rendering_capable && ( surface_format_count > 0 ) && ( present_mode_count > 0 );
    }

    std::vector<device::queue_family_info> device::get_queue_family_infos( VkPhysicalDevice gpu ) const
    {
        std::vector<queue_family_info> queue_family_infos;

        std::uint32_t queue_property_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( gpu, &queue_property_count, nullptr );
        VkQueueFamilyProperties* p_queue_family_properties = reinterpret_cast<VkQueueFamilyProperties*>( alloca( sizeof( VkQueueFamilyProperties ) * queue_property_count ) );
        vkGetPhysicalDeviceQueueFamilyProperties( gpu, &queue_property_count, p_queue_family_properties );

        for ( size_t i = 0; i < queue_property_count; ++i )
        {
            if ( p_queue_family_properties[i].queueCount > 0 )
            {
                if ( p_queue_family_properties[i].queueFlags == VK_QUEUE_TRANSFER_BIT )
                {
                    const auto info = queue_family_info( )
                        .set_queue_count( p_queue_family_properties[i].queueCount )
                        .set_transfer( i );

                    queue_family_infos.push_back( info );
                }

                VkBool32 is_surface_supported;
                vkGetPhysicalDeviceSurfaceSupportKHR( gpu, i, surface_, &is_surface_supported );

                if ( ( p_queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ) && 
                    ( p_queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT ) && 
                    ( is_surface_supported == VK_TRUE ) )
                {
                    const auto info = queue_family_info( )
                        .set_queue_count( p_queue_family_properties[i].queueCount )
                        .set_graphics( i )
                        .set_present( i )
                        .set_transfer( i );

                    queue_family_infos.push_back( info );
                }
            }
        }

        return queue_family_infos;
    }

    /* Queue Family Info function implementation */
    device::queue_family_info::queue_family_info( 
        const std::uint32_t queue_count,
        const std::optional<std::uint32_t> graphics,
        const std::optional<std::uint32_t> present,
        const std::optional<std::uint32_t> transfer )
        :
        queue_count_( queue_count ),
        graphics_( graphics ),
        present_( present ),
        transfer_( transfer )
    {   }

    device::queue_family_info& device::queue_family_info::set_queue_count( const std::uint32_t count ) noexcept
    {
        queue_count_ = count;
        return *this;
    }
    device::queue_family_info& device::queue_family_info::set_graphics( const std::uint32_t index ) noexcept
    {
        graphics_ = index;
        return *this;
    }
    device::queue_family_info& device::queue_family_info::set_present( const std::uint32_t index ) noexcept
    {
        present_ = index;
        return *this;
    }
    device::queue_family_info& device::queue_family_info::set_transfer( const std::uint32_t index ) noexcept 
    {
        transfer_ = index;
        return *this;
    }

    bool device::queue_family_info::has_rendering_support( ) const noexcept
    {
        return graphics_.has_value( ) && present_.has_value( );
    }
    bool device::queue_family_info::is_general_purpose( ) const noexcept
    {
        return has_rendering_support( ) && present_.has_value( ) && transfer_.has_value( );
    }
    bool device::queue_family_info::is_dedicated_transfer( ) const noexcept
    {
        return transfer_.has_value( ) && !graphics_.has_value( ) && !present_.has_value( );
    }
    bool device::queue_family_info::is_dedicated_graphics( ) const noexcept
    {
        return graphics_.has_value( ) && !transfer_.has_value( );
    }
}
