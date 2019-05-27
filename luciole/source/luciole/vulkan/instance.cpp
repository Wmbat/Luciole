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

#include "instance.hpp"

#include "../utilities/log.hpp"

namespace lcl::vulkan
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback_function( 
        VkDebugReportFlagsEXT flags, 
        VkDebugReportObjectTypeEXT objType, 
        uint64_t obj, size_t location, 
        int32_t code, const char *layerPrefix, 
        const char *msg, void *userData )
    {
        if ( flags & VK_DEBUG_REPORT_WARNING_BIT_EXT )
        {
            core_warn( "Validation Layers -> {0}.", msg );
        }
        else if ( flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT )
        {
            core_warn( "Validation Layers -> {0}.", msg );
        }
        else if ( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT )
        {
            core_error( "Validation Layers -> {0}.", msg );
        }
        
        return VK_FALSE;
    }

    VkResult create_debug_report_callback( 
        VkInstance instance,
        const VkDebugReportCallbackCreateInfoEXT* p_create_info,
        const VkAllocationCallbacks* p_allocator,
        VkDebugReportCallbackEXT* p_callback ) 
    {
        auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>( vkGetInstanceProcAddr( instance, "vkCreateDebugReportCallbackEXT" ) );
        
        if (func != nullptr) 
        {
            return func( instance, p_create_info, p_allocator, p_callback );
        } 
        else 
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void destroy_debug_report_callback(
        VkInstance instance,
        VkDebugReportCallbackEXT callback,
        const VkAllocationCallbacks* p_allocator )
    {
        auto func = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>( vkGetInstanceProcAddr( instance, "vkDestroyDebugReportCallbackEXT" ) );

        if ( func != nullptr )
        {
            func( instance, callback, p_allocator );
        }
    }

    instance::instance( )
    {        
        std::uint32_t api_version;
        vkEnumerateInstanceVersion( &api_version );

        core_info( "Using Vulkan {}.{}.{}", VK_VERSION_MAJOR( api_version ), VK_VERSION_MINOR( api_version ), VK_VERSION_PATCH( api_version ) );

        std::vector<const char*> validation_layers;
        if constexpr ( vulkan::enable_debug_layers )
        {
            validation_layers.emplace_back( "VK_LAYER_LUNARG_standard_validation" );
        }
        
        std::uint32_t extension_count = 0;
        vkEnumerateInstanceExtensionProperties( nullptr, &extension_count, nullptr );
        VkExtensionProperties* p_extensions = reinterpret_cast<VkExtensionProperties*>( alloca( sizeof( VkExtensionProperties ) * extension_count ) );
        vkEnumerateInstanceExtensionProperties( nullptr, &extension_count, p_extensions );

        for ( size_t i = 0; i < extension_count; ++i )
        {
            for ( const auto& extension : extensions_.extensions_ )
            {
                if ( strcmp( extension.name_, p_extensions[i].extensionName ) == 0 )
                {
                    extensions_.enable( p_extensions[i].extensionName );
                }
            }
        }

        if ( !extensions_.is_core_enabled( ) )
        {
            core_error( "Some Extensions are not supported." );
        }

        check_layer_support( validation_layers );

        const auto enabled_extensions = extensions_.get_enabled_extension_names( );
        const auto enabled_layers = layers_.get_enabled_layers( );
        
        for( const char* extension : enabled_extensions )
        {
            core_info( "Instance extension \"{}\" enabled.", std::string{ extension } );
        }

        for( const char* layer : enabled_layers )
        {
            core_info( "Instance Layer \"{}\" enabled.", std::string{ layer } );
        }

        VkApplicationInfo app_info = { };
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.apiVersion = api_version;
        app_info.engineVersion = VK_MAKE_VERSION( 0, 0, 6 );
        app_info.pEngineName = "Luciole";

        if constexpr( enable_debug_layers )
        {
            VkInstanceCreateInfo create_info = { };
            create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            create_info.pApplicationInfo = &app_info;
            create_info.enabledExtensionCount = static_cast<std::uint32_t>( enabled_extensions.size( ) );
            create_info.ppEnabledExtensionNames = enabled_extensions.data( );
            create_info.enabledLayerCount = static_cast<std::uint32_t>( enabled_layers.size( ) );
            create_info.ppEnabledLayerNames = enabled_layers.data( );

            if ( vkCreateInstance( &create_info, nullptr, &handle_ ) != VK_SUCCESS )
            {
                core_error( "Failed to create Instance" );
            };
        }
        else
        {
            VkInstanceCreateInfo create_info = { };
            create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            create_info.pApplicationInfo = &app_info;
            create_info.enabledExtensionCount = static_cast<std::uint32_t>( enabled_extensions.size( ) );
            create_info.ppEnabledExtensionNames = enabled_extensions.data( );
            create_info.enabledLayerCount = 0;
            create_info.ppEnabledLayerNames = nullptr;
            

            if ( vkCreateInstance( &create_info, nullptr, &handle_ ) != VK_SUCCESS )
            {
                core_error( "Failed to create Instance" );
            }
        }

        if constexpr ( enable_debug_layers )
        {
            VkDebugReportCallbackCreateInfoEXT create_info = { };
            create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
            create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
            create_info.pfnCallback = debug_callback_function;

            if ( create_debug_report_callback( handle_, &create_info, nullptr, &debug_report_callback_ ) != VK_SUCCESS )
            {
                core_error( "Failed to create Debug Report callback" );
            }
        }
    }
    instance::instance( instance&& other ) noexcept
    {
        *this = std::move( other );
    }
    instance::~instance( )
    {
        if ( debug_report_callback_ != VK_NULL_HANDLE )
            destroy_debug_report_callback( handle_, debug_report_callback_, nullptr );

        if ( handle_ != VK_NULL_HANDLE )
            vkDestroyInstance( handle_, nullptr );
    }

    instance& instance::operator=( instance&& rhs ) noexcept
    {
        if ( this != &rhs )
        {
            handle_ = rhs.handle_;
            rhs.handle_ = VK_NULL_HANDLE;

            debug_report_callback_ = rhs.debug_report_callback_;
            rhs.debug_report_callback_ = VK_NULL_HANDLE;
        }

        return *this;
    }


    void instance::check_layer_support( const std::vector<const char*>& desired_layers )
    {
        std::uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties( &layer_count, nullptr );
        VkLayerProperties* p_properties = reinterpret_cast<VkLayerProperties*>( alloca( sizeof( VkLayerProperties ) * layer_count ) );
        vkEnumerateInstanceLayerProperties( &layer_count, p_properties );

        for( const char* layer : desired_layers )
        {
            for ( size_t i = 0; i < layer_count; ++i )
            {
                if ( strcmp( p_properties[i].layerName, layer ) == 0 )
                {
                    layers_.enable( layer );
                }
            }
        }
    }
}