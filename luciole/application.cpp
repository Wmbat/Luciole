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

#include <map>

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
    p_wnd_ = std::make_unique<win32_window>( "Luciole" );
#elif defined( VK_USE_PLATFORM_XCB_KHR )
    p_wnd_ = std::make_unique<xcb_window>( "Luciole" );
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
    create_surface( );
    pick_gpu( );
    create_device( );
    create_swapchain( );
    create_image_views( );
}
application::~application( )
{
    destroy_image_views( );
    destroy_swapchain( );
    destroy_device( );
    destroy_surface( );
    destroy_debug_messenger( );
    destroy_instance( );
}

void application::run( )
{
    while( p_wnd_->is_open( ) )
    {

        p_wnd_->poll_events( );
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

        if ( create_debug_utils_messenger( instance_, &debug_messenger_create_info, nullptr, &debug_messenger_ ) != VK_SUCCESS ) 
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

void application::create_surface( )
{
    surface_ = p_wnd_->create_surface( instance_ );
}
void application::destroy_surface( )
{
    if ( surface_ != VK_NULL_HANDLE )
    {
        vkDestroySurfaceKHR( instance_, surface_, nullptr );
        surface_ = VK_NULL_HANDLE;
    }
}

void application::pick_gpu( )
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
    }
    else
    {
        core_error( "Failed to find a suitable GPU" );
    }
}

int application::rate_gpu( const VkPhysicalDevice gpu )
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

void application::create_device( )
{
    std::uint32_t properties_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( gpu_, &properties_count, nullptr );
    VkQueueFamilyProperties* queue_family_properties = reinterpret_cast<VkQueueFamilyProperties*>( alloca( sizeof( VkQueueFamilyProperties ) * properties_count ) );
    vkGetPhysicalDeviceQueueFamilyProperties( gpu_, &properties_count, queue_family_properties );

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

    for( size_t i = 0; i < properties_count; ++i )
    {
        if ( queue_family_properties[i].queueCount > 0 )
        {
            VkDeviceQueueCreateInfo create_info
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = static_cast<std::uint32_t>( i ),
                .queueCount = queue_family_properties[i].queueCount,
                .pQueuePriorities = nullptr
            };

            queue_create_infos.emplace_back( create_info );
        }
    }

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures( gpu_, &features );

    std::uint32_t extension_count = 0;
    vkEnumerateDeviceExtensionProperties( gpu_, nullptr, &extension_count, nullptr );
    VkExtensionProperties* extensions = reinterpret_cast<VkExtensionProperties*>( alloca( sizeof( VkExtensionProperties ) * extension_count ) );
    vkEnumerateDeviceExtensionProperties( gpu_, nullptr, &extension_count, extensions );

    for( size_t i = 0; i < extension_count; ++i )
    {
        for( auto& extension : device_extensions_ )
        {
            if ( strcmp( extensions[i].extensionName, extension.name_.c_str( ) ) ) 
            {
                extension.found_ = true;
            }
        }
    }

    std::vector<const char*> enabled_extensions;
    enabled_extensions.reserve( device_extensions_.size( ) );

    bool not_supported = false;
    for( const auto& extension : device_extensions_ )
    {
        if ( ( !extension.found_ ) && extension.priority_ == extension::priority::e_required )
        {
            not_supported = true;
        }

        if ( extension.found_ )
        {
            enabled_extensions.emplace_back( extension.name_.c_str( ) );

            core_info( "Device Extension \"{}\" ENABLED .", extension.name_ );
        }
    }

    if ( not_supported )
    {
        core_error( "Required Extension not supported" );

        throw;
    }

    const VkDeviceCreateInfo create_info
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<std::uint32_t>( queue_create_infos.size( ) ),
        .pQueueCreateInfos = queue_create_infos.data( ),
        .enabledExtensionCount = static_cast<std::uint32_t>( enabled_extensions.size( ) ),
        .ppEnabledExtensionNames = enabled_extensions.data( ),
        .pEnabledFeatures = &features
    };

    if ( vkCreateDevice( gpu_, &create_info, nullptr, &device_ ) != VK_NULL_HANDLE )
    {
        core_error( "Failed to create Device." );
    }

    bool has_transfer_only_ = false;
    bool has_compute_only_ = false;
    for( size_t i = 0; i < properties_count; ++i )
    {
        if ( queue_family_properties[i].queueCount > 0 )
        {
            if ( queue_family_properties[i].queueFlags == VK_QUEUE_TRANSFER_BIT )
            {
                vkGetDeviceQueue( device_, i, 0, &transfer_queue_ );

                has_transfer_only_ = true;
            }

            if ( queue_family_properties[i].queueFlags == VK_QUEUE_COMPUTE_BIT )
            {
                vkGetDeviceQueue( device_, i, 0, &compute_queue_ );

                has_compute_only_ = true;
            }
        }
    }

    for( size_t i = 0; i < properties_count; ++i )
    {
        if ( queue_family_properties[i].queueCount > 0 )
        {
            if ( queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT && 
                 queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
                 queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT )
            {
                std::uint32_t index = 0;

                vkGetDeviceQueue( device_, i, index, &graphics_queue_ );
                ++index;

                if ( !has_transfer_only_ )
                {
                    vkGetDeviceQueue( device_, i, index, &transfer_queue_ );
                    ++index;
                }

                if ( !has_compute_only_ )
                {
                    vkGetDeviceQueue( device_, i, index, &compute_queue_ );
                    ++index;
                }
            }
        }
    }
}
void application::destroy_device( )
{
    if ( device_ != VK_NULL_HANDLE )
    {
        vkDestroyDevice( device_, nullptr );
        device_ = VK_NULL_HANDLE;
    }
}

void application::create_swapchain( )
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( gpu_, surface_, &capabilities );
    
    auto surface_format = choose_swapchain_surface_format( );
    auto present_mode = choose_swapchain_present_mode( );
    auto extent = choose_swapchain_extent( capabilities );

    std::uint32_t image_count = capabilities.minImageCount + 1;
    if ( capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount )
    {
        image_count = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info 
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = {},
        .surface = surface_,
        .minImageCount = image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = nullptr
    };

    if ( vkCreateSwapchainKHR( device_, &create_info, nullptr, &swapchain_ ) != VK_SUCCESS )
    {
        core_error( "Failed to create swapchain.");
    }

    vkGetSwapchainImagesKHR( device_, swapchain_, &image_count, nullptr );
    swapchain_images.resize( image_count );
    vkGetSwapchainImagesKHR( device_, swapchain_, &image_count, swapchain_images.data( ) );

    swapchain_image_format_ = surface_format.format;
    swapchain_extent_ = extent;
}
void application::destroy_swapchain( )
{
    if ( swapchain_ != VK_NULL_HANDLE )
    {
        vkDestroySwapchainKHR( device_, swapchain_, nullptr );
        swapchain_ = VK_NULL_HANDLE;
    }
}

void application::create_image_views( )
{
    swapchain_image_views.resize( swapchain_images.size( ) );

    for ( size_t i = 0; i < swapchain_image_views.size( ); ++i )
    {
        VkImageViewCreateInfo create_info 
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
            .image = swapchain_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapchain_image_format_,
            .components = VkComponentMapping
            { 
                .r = VK_COMPONENT_SWIZZLE_IDENTITY, 
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY, 
                .a = VK_COMPONENT_SWIZZLE_IDENTITY 
            },
            .subresourceRange = VkImageSubresourceRange
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        if ( vkCreateImageView( device_, &create_info, nullptr, &swapchain_image_views[i] ) != VK_SUCCESS )
        {
            core_error( "Failed to create image views!" );
        }
    }
}
void application::destroy_image_views( )
{

}

VkSurfaceFormatKHR application::choose_swapchain_surface_format( )
{
    std::uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR( gpu_, surface_, &format_count, nullptr );
    VkSurfaceFormatKHR* p_surface_formats = reinterpret_cast<VkSurfaceFormatKHR*>( alloca( sizeof( VkSurfaceFormatKHR ) * format_count ) );
    vkGetPhysicalDeviceSurfaceFormatsKHR( gpu_, surface_, &format_count, p_surface_formats ); 

    for ( size_t i = 0; i < format_count; ++i )
    {
        if ( p_surface_formats[i].format == VK_FORMAT_B8G8R8A8_UNORM && p_surface_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
        {
            return p_surface_formats[i];
        }
    }

    return p_surface_formats[0];
}

VkPresentModeKHR application::choose_swapchain_present_mode( )
{
    std::uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR( gpu_, surface_, &present_mode_count, nullptr );
    VkPresentModeKHR* p_present_modes = reinterpret_cast<VkPresentModeKHR*>( alloca( sizeof( VkPresentModeKHR ) * present_mode_count ) );
    vkGetPhysicalDeviceSurfacePresentModesKHR( gpu_, surface_, &present_mode_count, p_present_modes );

    for ( size_t i = 0; i < present_mode_count; ++i )
    {
        if ( p_present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR )
        {
            return p_present_modes[i];
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D application::choose_swapchain_extent( const VkSurfaceCapabilitiesKHR& capabilities )
{
    if ( capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max( ) )
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actual_extent = { p_wnd_->get_width( ), p_wnd_->get_height( ) };

        actual_extent.width = std::clamp( actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
        actual_extent.height = std::clamp( actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );
    
        return actual_extent;
    }
}