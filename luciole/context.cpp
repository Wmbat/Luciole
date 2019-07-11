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

#include "utilities/log.hpp"

namespace lcl::core
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
        void* p_user_data ) 
    {
        if ( message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT )
        {
            core_info( "Validation Layer Info: {}", p_callback_data->pMessage );
        }
        else if ( message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT )
        {
            core_warn( "Validation Layer Warning: {}", p_callback_data->pMessage );
        }
        else if ( message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT )
        {
            core_error( "Validation Layer Error: {}", p_callback_data->pMessage );
        }
        
        return VK_FALSE;
    }

    VkResult create_debug_utils_messenger (
        VkInstance instance, 
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, 
        VkDebugUtilsMessengerEXT* pDebugMessenger ) 
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        if ( func != nullptr ) 
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } 
        else 
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void destroy_debug_utils_messenger ( 
        VkInstance instance, 
        VkDebugUtilsMessengerEXT debugMessenger, 
        const VkAllocationCallbacks* pAllocator ) 
    {
        auto func = ( PFN_vkDestroyDebugUtilsMessengerEXT ) vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );

        if (func != nullptr) 
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    const VkPhysicalDevice context::get_physical_device( ) const
    {
        return gpu_;
    }

    void context::create_instance( const VkApplicationInfo& app_info ) noexcept
    {
        // Get Instance Extensions
        std::uint32_t instance_extension_count = 0;
        vkEnumerateInstanceExtensionProperties( nullptr, &instance_extension_count, nullptr );
        VkExtensionProperties* extension_properties = reinterpret_cast<VkExtensionProperties*>( alloca( sizeof( VkExtensionProperties ) * instance_extension_count ) );
        vkEnumerateInstanceExtensionProperties( nullptr, &instance_extension_count, extension_properties );

        instance_extensions_.reserve( instance_extension_count );
        for ( size_t i = 0; i < instance_extension_count; ++i )
        {
            instance_extensions_.push_back( extension_properties[i].extensionName );

            core_info( "Instance Extension \"{}\" ENABLED .", extension_properties[i].extensionName );
        }


        if constexpr ( core::enable_debug_layers )
        {
            std::uint32_t layer_count = 0;
            vkEnumerateInstanceLayerProperties( &layer_count, nullptr );
            VkLayerProperties* layer_properties = reinterpret_cast<VkLayerProperties*>( alloca( sizeof( VkLayerProperties ) * layer_count ) );
            vkEnumerateInstanceLayerProperties( &layer_count, layer_properties );



            const VkInstanceCreateInfo create_info 
            {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pApplicationInfo = &app_info,
                .enabledLayerCount = static_cast<std::uint32_t>( validation_layers.size( ) ),
                .ppEnabledLayerNames = validation_layers.data( ),
                .enabledExtensionCount = static_cast<std::uint32_t>( instance_extensions_.size( ) ),
                .ppEnabledExtensionNames = instance_extensions_.data( )
            };

            if ( vkCreateInstance( &create_info, nullptr, &instance_ ) != VK_SUCCESS )
            {
                core_error( "Failed to create Vulkan Instance" );
            }
        }
        else
        {
            const VkInstanceCreateInfo create_info 
            {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pApplicationInfo = &app_info,
                .enabledLayerCount = 0,
                .ppEnabledLayerNames = nullptr,
                .enabledExtensionCount = static_cast<std::uint32_t>( instance_extensions_.size( ) ),
                .ppEnabledExtensionNames = instance_extensions_.data( )
            };

            if ( vkCreateInstance( &create_info, nullptr, &instance_ ) != VK_SUCCESS ) 
            {
                core_error( "Failed to create Vulkan Instance" );
            }
        }
    }

    void context::destroy_instance( ) noexcept
    {
        if ( instance_ != VK_NULL_HANDLE )
        {
            vkDestroyInstance( instance_, nullptr );
            instance_ = VK_NULL_HANDLE;
        }
    }

    void context::create_debug_messenger( ) noexcept
    {
        if constexpr ( core::enable_debug_layers ) 
        {
            const VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info 
            {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = debug_callback,
                .pUserData = nullptr
            };

            if ( create_debug_utils_messenger( instance_, &debug_messenger_create_info, nullptr, &debug_messenger_ ) != VK_SUCCESS ) 
            {
                core_error( "Failed to create Vulkan Debug Messenger" );
            }
        }
    }
    void context::destroy_debug_messenger( ) noexcept
    {
        if constexpr ( core::enable_debug_layers )
        {
            if ( debug_messenger_ != VK_NULL_HANDLE )
            {
                destroy_debug_utils_messenger( instance_, debug_messenger_, nullptr );
                debug_messenger_ = VK_NULL_HANDLE;
            }
        } 
    }

    void context::destroy_device( ) noexcept
    {
        if ( device_ != VK_NULL_HANDLE )
        {
            vkDestroyDevice( device_, nullptr );
            device_ = VK_NULL_HANDLE;
        }
    }
}