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

#include <cstring>
#include <map>

#include "utilities/log.hpp"

#include "context.hpp"
#include "luciole_core.hpp"

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

context::context( const window& wnd )
{
    wnd_size_ = { wnd.get_width(), wnd.get_height() };
    
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

    validation_layers_ = load_validation_layers( );  
    instance_extensions_ = load_instance_extensions( );

    const auto layer_names = check_layer_support( layers_t( validation_layers_ ) );
    if constexpr( enable_debug_layers )
    {
        if ( layer_names.empty( ) )
        {
            core_error( "1 or more validation layers are not supported." );
            throw;
        }
        else
        {
            for ( auto const& name : layer_names )
            {
                core_info( "Validation Layer \"{}\" ENABLED.", name );
            }
        }
        
    }

    const auto instance_ext_names = check_ext_support( extensions_t( instance_extensions_ ) );
    if ( instance_ext_names.empty( ) )
    {
        core_error( "1 or more instance extensions are not supported." );
        throw;
    }
    else
    {
        for( auto const& name : instance_ext_names )
        {
            core_info( "Instance Extension \"{}\" ENABLED.", name );
        }
    }

    instance_ = vk_check(
        vk_instance_t( create_instance( app_info, extension_names_t( instance_ext_names ), layer_names_t( layer_names ) ) ),
        error_msg_t( "Failed to create Instance." ) );

    if constexpr ( enable_debug_layers )
    {
        debug_messenger_ = vk_check(
            vk_debug_messenger_t( create_debug_messenger( ) ),
            error_msg_t( "Failed to create Debug Utils Messenger!" ) );
    }

    surface_ = vk_check(
        vk_surface_t( create_surface( wnd ) ),
        error_msg_t( "Failed to create Surface!" ) );
    
    gpu_ = vk_check(
        vk_physical_device_t( pick_gpu( ) ),
        error_msg_t( "Failed to find a suitable Physical Device!" ) );

    device_extensions_ = load_device_extensions( );

    const auto queue_properties = query_queue_family_properties( );
    const auto device_ext_names = check_ext_support( extensions_t( device_extensions_ ) );
    if ( device_ext_names.empty( ) )
    {
        core_error( "1 or more device extensions are not supported!" );
        throw;
    }

    device_ = vk_check( 
        vk_device_t ( create_device( extension_names_t( device_ext_names ), queue_properties_t( queue_properties ) ) ),
        error_msg_t( "Failed to create Logical Device!" ) );

    queues_ = get_queues( queue_properties_t( queue_properties ) );

    command_pools_ = vk_check_array( create_command_pools( ), error_msg_t( "Failed to create 1 or more Command pools" ) );
}
context::context( context&& other )
{
    *this = std::move( other );
}
context::~context( )
{
    for ( auto& command_pool : command_pools_ )
    {
        if ( command_pool.handle_ != VK_NULL_HANDLE )
        {
            vkDestroyCommandPool( device_, command_pool.handle_, nullptr );
            command_pool.handle_ = VK_NULL_HANDLE;
            command_pool.family_ = 0;
            command_pool.flags_ = 0;
        }
    }
    
    if ( device_ != VK_NULL_HANDLE )
    {
        vkDestroyDevice( device_, nullptr );
        device_ = VK_NULL_HANDLE;
    }

    if ( surface_ != VK_NULL_HANDLE )
    {
        vkDestroySurfaceKHR( instance_, surface_, nullptr );
        surface_ = VK_NULL_HANDLE;
    }

    if constexpr ( enable_debug_layers )
    {
        if ( debug_messenger_ != VK_NULL_HANDLE )
        {
            destroy_debug_utils_messenger( instance_, debug_messenger_, nullptr );
            debug_messenger_ = VK_NULL_HANDLE;
        }
    }

    if ( instance_ != VK_NULL_HANDLE )
    {
        vkDestroyInstance( instance_, nullptr );
        instance_ = VK_NULL_HANDLE;
    }
}

context& context::operator=( context&& rhs )
{
    if ( this != &rhs )
    {
        std::swap( instance_, rhs.instance_ );
        std::swap( debug_messenger_, rhs.debug_messenger_ );
        std::swap( surface_, rhs.surface_ );
        std::swap( gpu_, rhs.gpu_ );
        std::swap( device_, rhs.device_ );
        
        std::swap( queues_, rhs.queues_ );
        std::swap( command_pools_, rhs.command_pools_ );

        std::swap( wnd_size_, rhs.wnd_size_ );
        
        validation_layers_ = std::move( rhs.validation_layers_ );
        instance_extensions_ = std::move( rhs.instance_extensions_ );
        device_extensions_ = std::move( rhs.device_extensions_ );
    }

    return *this;
}

VkSwapchainCreateInfoKHR context::swapchain_create_info( ) const noexcept
{
    VkSwapchainCreateInfoKHR const create_info
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = surface_
    };
    
    return create_info;
}

VkSwapchainKHR context::create_swapchain( vk_swapchain_create_info_t create_info ) const noexcept
{
    if ( VkSwapchainKHR handle = VK_NULL_HANDLE; vkCreateSwapchainKHR( device_, &create_info.value_, nullptr, &handle ) != VK_SUCCESS )
    {
        return VK_NULL_HANDLE;
    }
    else
    {
        return handle;
    }
}
void context::destroy_swapchain( VkSwapchainKHR swapchain ) const noexcept
{
    vkDestroySwapchainKHR( device_, swapchain, nullptr );
}

VkImageView context::create_image_view( vk_image_view_create_info_t create_info ) const noexcept
{
    if ( VkImageView handle = VK_NULL_HANDLE; vkCreateImageView( device_, &create_info.value_, nullptr, &handle ) != VK_SUCCESS )
    {
        return VK_NULL_HANDLE;
    }
    else
    {
        return handle;
    }
}

void context::destroy_image_view( vk_image_view_t image_view ) const noexcept
{
    vkDestroyImageView( device_, image_view.value_, nullptr );
}

VkRenderPass context::create_render_pass( vk_render_pass_create_info_t create_info ) const noexcept
{
    VkRenderPass handle = VK_NULL_HANDLE;
    
    if ( vkCreateRenderPass( device_, &create_info.value_, nullptr, &handle ) != VK_SUCCESS )
    {
        return VK_NULL_HANDLE;
    }
    else
    {
        return handle;
    }
}

void context::destroy_render_pass( vk_render_pass_t render_pass ) const noexcept
{
    if ( render_pass.value_ == VK_NULL_HANDLE )
    {
        vkDestroyRenderPass( device_, render_pass.value_, nullptr );
    }
}

std::vector<VkImage> context::get_swapchain_images( vk_swapchain_t swapchain, count32_t image_count ) const
{
    if ( vkGetSwapchainImagesKHR( device_, swapchain.value_, &image_count.value_, nullptr ) != VK_SUCCESS )
    {
        return { };
    }
    
    std::vector<VkImage> images( image_count.value_ );
    if ( vkGetSwapchainImagesKHR( device_, swapchain.value_, &image_count.value_, images.data() ) != VK_SUCCESS )
    {
        return { };
    }
    
    return images;
}

VkSurfaceCapabilitiesKHR context::get_surface_capabilities( ) const noexcept
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( gpu_, surface_, &capabilities );
    
    return capabilities;
}

std::vector<VkSurfaceFormatKHR> context::get_surface_format( ) const
{
    std::uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR( gpu_, surface_, &format_count, nullptr );
    std::vector<VkSurfaceFormatKHR> formats( format_count );
    vkGetPhysicalDeviceSurfaceFormatsKHR( gpu_, surface_, &format_count, formats.data() );
    
    return formats;
}

std::vector<VkPresentModeKHR> context::get_present_modes( ) const
{
    std::uint32_t mode_count = 0u;
    vkGetPhysicalDeviceSurfacePresentModesKHR( gpu_, surface_, &mode_count, nullptr );
    std::vector<VkPresentModeKHR> present_modes( mode_count );
    vkGetPhysicalDeviceSurfacePresentModesKHR( gpu_, surface_, &mode_count, present_modes.data() );
    
    return present_modes;
}

VkExtent2D context::get_window_extent( ) const
{
    return VkExtent2D{ wnd_size_.x, wnd_size_.y };
}

std::vector<layer> context::load_validation_layers( ) const
{
    if constexpr( enable_debug_layers )
    {
        std::vector<layer> layers =
        {
            layer{ .priority_ = layer::priority::e_optional, .found_ = false, .name_ = "VK_LAYER_KHRONOS_validation" }
        };

        std::uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties( &layer_count, nullptr );
        VkLayerProperties* layer_properties = reinterpret_cast<VkLayerProperties*>( alloca( sizeof( VkLayerProperties ) * layer_count ) );
        vkEnumerateInstanceLayerProperties( &layer_count, layer_properties );

        for( size_t i = 0; i < layer_count; ++i )
        {
            for( auto& layer : layers )
            {
                if ( strcmp( layer.name_.c_str( ), layer_properties[i].layerName ) == 0 )
                {
                    layer.found_ = true;
                }
            }
        }

        return layers;
    }
    else
    {
        return { };
    }
}

std::vector<extension> context::load_instance_extensions( ) const
{
    std::vector<extension> exts = 
    {
#if defined( VK_USE_PLATFORM_WIN32_KHR )
        extension{ .priority_ = extension::priority::e_required, .found_ = false, .name_ = "VK_KHR_win32_surface" },
#elif defined( VK_USE_PLATFORM_XCB_KHR )
        extension{ .priority_ = extension::priority::e_required, .found_ = false, .name_ = "VK_KHR_xcb_surface" },
#endif
        extension{ .priority_ = extension::priority::e_required, .found_ = false, .name_ = "VK_KHR_surface" },
        extension{ .priority_ = extension::priority::e_optional, .found_ = false, .name_ = "VK_KHR_load_surface_capabilities2" },
        extension{ .priority_ = extension::priority::e_optional, .found_ = false, .name_ = "VK_EXT_debug_utils" }
    };

    std::uint32_t instance_extension_count = 0;
    vkEnumerateInstanceExtensionProperties( nullptr, &instance_extension_count, nullptr );
    VkExtensionProperties* extension_properties = reinterpret_cast<VkExtensionProperties*>( alloca( sizeof( VkExtensionProperties ) * instance_extension_count ) );
    vkEnumerateInstanceExtensionProperties( nullptr, &instance_extension_count, extension_properties );

    for ( size_t i = 0; i < instance_extension_count; ++i )
    {
        for( auto& extension : exts )
        {
            if ( strcmp( extension.name_.c_str( ), extension_properties[i].extensionName ) == 0 )
            {
                extension.found_ = true;
            }
        }
    }

    return exts;
}

std::vector<extension> context::load_device_extensions( ) const
{
    std::vector<extension> exts =
    {
        extension{ .priority_ = extension::priority::e_required, .found_ = false, .name_ = "VK_KHR_swapchain" }
    };

    std::uint32_t extension_count = 0;
    vkEnumerateDeviceExtensionProperties( gpu_, nullptr, &extension_count, nullptr );
    VkExtensionProperties* extensions = reinterpret_cast<VkExtensionProperties*>( alloca( sizeof( VkExtensionProperties ) * extension_count ) );
    vkEnumerateDeviceExtensionProperties( gpu_, nullptr, &extension_count, extensions );

    for( size_t i = 0; i < extension_count; ++i )
    {
        for( auto& extension : exts )
        {
            if ( strcmp( extensions[i].extensionName, extension.name_.c_str( ) ) ) 
            {
                extension.found_ = true;
            }
        }
    }

    return exts;
}

std::vector<std::string> context::check_layer_support( const layers_t& layers ) const
{
    std::vector<std::string> enabled_layers;
    enabled_layers.reserve( validation_layers_.size( ) );

    for ( const auto& layer : validation_layers_ )
    {
        if ( ( !layer.found_ ) && layer.priority_ == layer::priority::e_required )
        {
            return { };
        }

        if ( layer.found_ )
        {
            enabled_layers.emplace_back( layer.name_ );
        }
    }

    enabled_layers.shrink_to_fit( );

    return enabled_layers;
}

std::vector<std::string> context::check_ext_support( const extensions_t& extensions ) const
{
    std::vector<std::string> enabled_extensions;
    enabled_extensions.reserve( extensions.value_.size( ) );

    for( const auto& extension : extensions.value_ )
    {
        if ( ( !extension.found_ ) && extension.priority_ == extension::priority::e_required )
        {
            return { };
        }

        if ( extension.found_ )
        {
            enabled_extensions.emplace_back( extension.name_ );
        }
    }

    enabled_extensions.shrink_to_fit( );

    return enabled_extensions;
}

VkInstance context::create_instance( const VkApplicationInfo& app_info, const extension_names_t& enabled_ext_name, const layer_names_t& enabled_layer_names ) const
{
    VkInstance handle = VK_NULL_HANDLE;

    std::vector<const char*> layers( enabled_layer_names.value_.size( ) );
    for( std::size_t i = 0; i < enabled_layer_names.value_.size( ); ++i )
    {
        layers[i] = enabled_layer_names.value_[i].c_str( );
    }

    std::vector<const char*> extensions( enabled_ext_name.value_.size( ) );
    for( std::size_t i = 0; i < enabled_ext_name.value_.size( ); ++i )
    {
        extensions[i] = enabled_ext_name.value_[i].c_str( );
    }

    if constexpr ( enable_debug_layers )
    {
        const VkInstanceCreateInfo create_info 
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, 
            .pApplicationInfo = &app_info,
            .enabledLayerCount = static_cast<uint32_t>( layers.size( ) ),
            .ppEnabledLayerNames = layers.data( ),
            .enabledExtensionCount = static_cast<std::uint32_t>( extensions.size( ) ),
            .ppEnabledExtensionNames = extensions.data( )
        };

        if ( vkCreateInstance( &create_info, nullptr, &handle ) != VK_SUCCESS )
        {
            return VK_NULL_HANDLE;
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
            .enabledExtensionCount = static_cast<std::uint32_t>( extensions.size( ) ),
            .ppEnabledExtensionNames = extensions.data( )
        };

        if ( vkCreateInstance( &create_info, nullptr, &handle ) != VK_SUCCESS )
        {
            return VK_NULL_HANDLE;
        }        
    }

    return handle;
}

VkDebugUtilsMessengerEXT context::create_debug_messenger( ) const
{
    VkDebugUtilsMessengerEXT handle = VK_NULL_HANDLE;

    const VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info 
    {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_callback,
        .pUserData = nullptr
    };

    if ( create_debug_utils_messenger( instance_, &debug_messenger_create_info, nullptr, &handle ) != VK_SUCCESS )
    {
        return VK_NULL_HANDLE;
    }
    
    return handle;
}

VkSurfaceKHR context::create_surface( const window& wnd ) const
{
    return wnd.create_surface( instance_ );
}

VkPhysicalDevice context::pick_gpu( ) const
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
        return candidates.begin( )->second;
    }

    return VK_NULL_HANDLE;
}

VkDevice context::create_device( const extension_names_t& enabled_ext_name, const queue_properties_t& queue_properties ) const
{
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    queue_create_infos.reserve( queue_properties.value_.size( ) );

    for( size_t i = 0; i < queue_properties.value_.size( ); ++i )
    {
        if ( queue_properties.value_[i].queueCount > 0 )
        {
            VkDeviceQueueCreateInfo create_info
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = static_cast<std::uint32_t>( i ),
                .queueCount = queue_properties.value_[i].queueCount,
                .pQueuePriorities = nullptr
            };

            queue_create_infos.emplace_back( create_info );
        }
    }

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures( gpu_, &features );

    VkDevice handle = VK_NULL_HANDLE;

    std::vector<const char*> extensions( enabled_ext_name.value_.size( ) );
    for( std::size_t i = 0; i < enabled_ext_name.value_.size( ); ++i )
    {
        extensions[i] = enabled_ext_name.value_[i].c_str( );
    }

    const VkDeviceCreateInfo create_info
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<std::uint32_t>( queue_create_infos.size( ) ),
        .pQueueCreateInfos = queue_create_infos.data( ),
        .enabledExtensionCount = static_cast<std::uint32_t>( extensions.size( ) ),
        .ppEnabledExtensionNames = extensions.data( ),
        .pEnabledFeatures = &features
    };

    if ( vkCreateDevice( gpu_, &create_info, nullptr, &handle ) != VK_NULL_HANDLE )
    {
        return VK_NULL_HANDLE;
    }

    return handle;
}

std::vector<queue> context::get_queues( const queue_properties_t& queue_properties ) const
{
    std::vector<queue> queues;
    queues.reserve( 3 );

    bool has_transfer_only_ = false;
    bool has_compute_only_ = false;
    for( size_t i = 0; i < queue_properties.value_.size( ); ++i )
    {
        if ( queue_properties.value_[i].queueCount > 0 )
        {
            if ( queue_properties.value_[i].queueFlags == VK_QUEUE_TRANSFER_BIT )
            {
                VkQueue handle = VK_NULL_HANDLE;
                vkGetDeviceQueue( device_, i, 0, &handle );

                struct queue transfer_queue
                {
                    .handle_ = handle,
                    .flags_ = VK_QUEUE_TRANSFER_BIT,
                    .family_ = static_cast<std::uint32_t>( i ),
                    .index_ = 0
                };

                queues.emplace_back( transfer_queue );

                has_transfer_only_ = true;
            }

            if ( queue_properties.value_[i].queueFlags == VK_QUEUE_COMPUTE_BIT )
            {
                VkQueue handle = VK_NULL_HANDLE;
                vkGetDeviceQueue( device_, i, 0, &handle );
            
                struct queue compute_queue
                {
                    .handle_ = handle,
                    .flags_ = VK_QUEUE_COMPUTE_BIT,
                    .family_ = static_cast<std::uint32_t>( i ),
                    .index_ = 0
                };

                queues.emplace_back( compute_queue );

                has_compute_only_ = true;
            }
        }
    }

    for( size_t i = 0; i < queue_properties.value_.size( ); ++i )
    {
        if ( queue_properties.value_[i].queueCount > 0 )
        {
            if ( queue_properties.value_[i].queueFlags & VK_QUEUE_TRANSFER_BIT && 
                 queue_properties.value_[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
                 queue_properties.value_[i].queueFlags & VK_QUEUE_COMPUTE_BIT )
            {
                std::uint32_t index = 0;

                VkQueue gfx_handle = VK_NULL_HANDLE;
                vkGetDeviceQueue( device_, i, index, &gfx_handle );
                
                struct queue gfx_queue
                {
                    .handle_ = gfx_handle,
                    .flags_ = VK_QUEUE_GRAPHICS_BIT,
                    .family_ = static_cast<std::uint32_t>( i ),
                    .index_ = index
                };

                queues.emplace_back( gfx_queue );

                ++index;

                if ( !has_transfer_only_ )
                {
                    VkQueue handle = VK_NULL_HANDLE;
                    vkGetDeviceQueue( device_, i, index, &handle );

                    struct queue transfer_queue
                    {
                        .handle_ = handle,
                        .flags_ = VK_QUEUE_TRANSFER_BIT,
                        .family_ = static_cast<std::uint32_t>( i ),
                        .index_ = index
                    };

                    queues.emplace_back( transfer_queue );

                    ++index;
                }

                if ( !has_compute_only_ )
                {
                    VkQueue handle = VK_NULL_HANDLE;
                    vkGetDeviceQueue( device_, i, index, &handle );
            
                    struct queue compute_queue
                    {
                        .handle_ = handle,
                        .flags_ = VK_QUEUE_COMPUTE_BIT,
                        .family_ = static_cast<std::uint32_t>( i ),
                        .index_ = index
                    };

                    queues.emplace_back( compute_queue );

                    ++index;
                }
            }
        }
    }

    queues.shrink_to_fit( );

    return queues;
}

std::vector<command_pool>context::create_command_pools( ) const
{
    std::vector<command_pool> command_pools;

    for ( const auto& queue : queues_ )
    {
        bool is_already_present = false;
        for ( const auto& command_pool : command_pools )
        {
            is_already_present = command_pool.family_ == queue.family_;
        }

        if ( !is_already_present )
        {
            VkCommandPool handle = VK_NULL_HANDLE;

            VkCommandPoolCreateInfo create_info 
            {
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .flags = 0,
                .queueFamilyIndex = queue.family_
            };

            if ( vkCreateCommandPool( device_, &create_info, nullptr, &handle ) != VK_SUCCESS )
            {
                return { };
            }

            struct command_pool command_pool 
            {
                .handle_ = handle,
                .family_ = queue.family_,
                .flags_ = queue.flags_
            };

            command_pools.push_back( command_pool );
        }
    }

    return command_pools;
}

std::vector<VkCommandBuffer> context::create_command_buffers( const VkCommandPool command_pool, count32_t count ) const
{
    std::vector<VkCommandBuffer> handles( count.value_ );
    
    const VkCommandBufferAllocateInfo allocate_info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = count.value_
    };
    
    if ( vkAllocateCommandBuffers( device_, &allocate_info, handles.data() ) != VK_SUCCESS )
    {
        return { };
    }
    
    return handles;
}


int context::rate_gpu( const VkPhysicalDevice gpu ) const
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

std::vector<VkQueueFamilyProperties> context::query_queue_family_properties( ) const
{
    std::uint32_t properties_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( gpu_, &properties_count, nullptr );
    std::vector<VkQueueFamilyProperties> properties( properties_count );
    vkGetPhysicalDeviceQueueFamilyProperties( gpu_, &properties_count, properties.data( ) );

    return properties;
}