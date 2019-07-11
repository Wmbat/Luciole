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

#include "application.hpp"

#include "utilities/log.hpp"

#if defined( VK_USE_PLATFORM_WIN32_KHR )
#include "window/win32_window.hpp"
#elif defined( VK_USE_PLATFORM_XCB_KHR )
#include "window/xcb_window.hpp"
#endif 

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


application::application( )
{
#if defined( VK_USE_PLATFORM_WIN32_KHR )
    wnd_ = std::make_unique<win32_window>( "Luciole" );
#elif defined( VK_USE_PLATFORM_XCB_KHR )
    wnd_ = std::make_unique<xcb_window>( "Luciole" );
#endif 

    std::uint32_t api_version;
    vkEnumerateInstanceVersion( &api_version );

    const VkApplicationInfo app_info
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "Luciole",
        .applicationVersion = VK_MAKE_VERSION( 0, 0, 0 ),
        .pEngineName = nullptr,
        .engineVersion = VK_MAKE_VERSION( 0, 0, 0 ),
        .apiVersion = api_version
    };

    create_instance( app_info );
    create_debug_messenger( );

}
application::~application( )
{
    destroy_debug_messenger( );
    destroy_instance( );
}

void application::run( )
{
    while( wnd_->is_open( ) )
    {

        wnd_->poll_events( );
    }
}

void application::create_instance( const VkApplicationInfo& app_info )
{
    std::uint32_t instance_extension_count = 0;
    vkEnumerateInstanceExtensionProperties( nullptr, &instance_extension_count, nullptr );
    VkExtensionProperties* extension_properties = reinterpret_cast<VkExtensionProperties*>( alloca( sizeof( VkExtensionProperties ) * instance_extension_count ) );
    vkEnumerateInstanceExtensionProperties( nullptr, &instance_extension_count, extension_properties );

    for ( size_t i = 0; i < instance_extension_count; ++i )
    {
        for( auto& extension : instance_extensions_ )
        {
            if ( strcmp( extension.name_.c_str( ), extension_properties[i].extensionName ) == 0 )
            {
                extension.found_ = true;
            }
        }
    }

    std::vector<const char*> enabled_extensions;
    enabled_extensions.reserve( instance_extensions_.size( ) );

    bool not_supported = false;
    for( const auto& extension : instance_extensions_ )
    {
        if ( ( !extension.found_ ) && extension.priority_ == extension::priority::e_required )
        {
            not_supported = true;
        }

        if ( extension.found_ )
        {
            enabled_extensions.emplace_back( extension.name_.c_str( ) );

            core_info( "Instance Extension \"{}\" ENABLED .", extension.name_ );
        }
    }

    if ( not_supported )
    {
        core_error( "Required Extension not supported" );

        throw;
    }

    if constexpr ( enable_debug_layers )
    {
        std::uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties( &layer_count, nullptr );
        VkLayerProperties* layer_properties = reinterpret_cast<VkLayerProperties*>( alloca( sizeof( VkLayerProperties ) * layer_count ) );
        vkEnumerateInstanceLayerProperties( &layer_count, layer_properties );

        bool layer_found = false;
        for ( const char* layer_name : validation_layers_ )
        {
            for ( size_t i = 0; i < layer_count; ++i )
            {
                if ( strcmp( layer_name, layer_properties[i].layerName ) == 0 )
                {
                    layer_found = true;
                    break;
                }
            }
        }

        if ( !layer_found )
        {
            core_error( "Failed to find validation layers" );
        }

        const VkInstanceCreateInfo create_info 
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, 
            .pApplicationInfo = &app_info,
            .enabledLayerCount = static_cast<std::uint32_t>( validation_layers_.size( ) ),
            .ppEnabledLayerNames = validation_layers_.data( ),
            .enabledExtensionCount = static_cast<std::uint32_t>( enabled_extensions.size( ) ),
            .ppEnabledExtensionNames = enabled_extensions.data( )
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
            .enabledExtensionCount = static_cast<std::uint32_t>( enabled_extensions.size( ) ),
            .ppEnabledExtensionNames = enabled_extensions.data( )
        };

        if ( vkCreateInstance( &create_info, nullptr, &instance_ ) != VK_SUCCESS )
        {
            core_error( "Failed to create Vulkan Instance" );
        }
    }
}
void application::destroy_instance( )
{
    if ( instance_ != VK_NULL_HANDLE )
    {
        vkDestroyInstance( instance_, nullptr );
        instance_ = VK_NULL_HANDLE;
    }
}

void application::create_debug_messenger( )
{
    if constexpr ( enable_debug_layers ) 
    {
        const VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info 
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debug_callback,
            .pUserData = nullptr
        };

        if (  create_debug_utils_messenger( instance_, &debug_messenger_create_info, nullptr, &debug_messenger_ ) != VK_SUCCESS ) 
        {
            core_error( "Failed to create Vulkan Debug Messenger" );
        }
    }
}
void application::destroy_debug_messenger( )
{
    if constexpr ( enable_debug_layers )
    {
        if ( debug_messenger_ != VK_NULL_HANDLE )
        {
            destroy_debug_utils_messenger( instance_, debug_messenger_, nullptr );
            debug_messenger_ = VK_NULL_HANDLE;
        }
    } 
}