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

#include <luciole/context.hpp>
#include <luciole/luciole_core.hpp>
#include <luciole/utilities/log.hpp>

#include <cstring>
#include <map>
#include <variant>

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
    void* p_user_data ) 
{
    if ( message_type != 0 )
    {

    }

    if ( p_user_data != nullptr )
    {

    }

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
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>( 
        vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" ) 
    );

    if ( func != nullptr ) 
    {
        return func( instance, pCreateInfo, pAllocator, pDebugMessenger );
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
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>( 
        vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" ) 
    );
    
    if (func != nullptr) 
    {
        func(instance, debugMessenger, pAllocator);
    }
}

context::context( const ui::window& wnd )
{
    wnd_size_ = wnd.get_size();
    
    std::uint32_t api_version;
    vkEnumerateInstanceVersion( &api_version );

    VkApplicationInfo const app_info
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

    auto const layer_names = check_layer_support( layers_t( validation_layers_ ) );
    if constexpr( vk::enable_debug_layers )
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

    auto const instance_ext_names = check_ext_support( extensions_t( instance_extensions_ ) );
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

    /*
     *  Check for any error on the instance creation.
     */
    if ( auto res = create_instance( app_info, extension_names_t( instance_ext_names ), layer_names_t( layer_names ) );
         auto p_val = std::get_if<VkInstance>( &res) )
    {
        instance_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: On Instance creation -> Error code: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }

    /*
     *  Check for any errors on the debug utils messenger creation.
     */ 
    if constexpr ( vk::enable_debug_layers )
    {
        if ( auto res = create_debug_messenger( ); auto p_val = std::get_if<VkDebugUtilsMessengerEXT>( &res ) )
        {
            debug_messenger_ = *p_val;
        }
        else
        {
            core_error( "Vulkan Error: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

            abort( );
        }
    }

    /*
     *  Check for any errors on the surface creation.
     */
    if ( auto res = create_surface( wnd ); auto p_val = std::get_if<VkSurfaceKHR>( &res ) )
    {
        surface_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }
    
    /*
     *  Check for any errors on the gpu picking.
     */
    if ( auto res = pick_gpu( ); auto p_val = std::get_if<VkPhysicalDevice>( &res ) )
    {
        gpu_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }

    device_extensions_ = load_device_extensions( );

    auto const queue_properties = query_queue_family_properties( );
    auto const device_ext_names = check_ext_support( extensions_t( device_extensions_ ) );
    if ( device_ext_names.empty( ) )
    {
        core_error( "1 or more device extensions are not supported!" );
        throw;
    }

    /*
     * Check for any errors on logical device creation.
     */
    if ( auto res = create_device( extension_names_t( device_ext_names ), queue_properties_t( queue_properties ) );
         auto p_val = std::get_if<VkDevice>( &res ) )
    {   
        device_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: On Device creation -> Error code: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }

    queues_ = get_queues( queue_properties_t( queue_properties ) );
    
    /*
     *  Check for any errors on the command pools creation.
     */
    if ( auto res = create_command_pools( ); auto p_val = std::get_if<command_pools_container_t>( &res ) )
    {
        command_pools_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }
    
}
context::context( context&& other )
{
    *this = std::move( other );
}
context::~context( )
{
    for ( auto& command_pool : command_pools_ )
    {
        if ( command_pool.second.handle_ != VK_NULL_HANDLE )
        {
            vkDestroyCommandPool( device_, command_pool.second.handle_, nullptr );
            command_pool.second.handle_ = VK_NULL_HANDLE;
            command_pool.second.flags_ = queue::flag::e_none;
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

    if constexpr ( vk::enable_debug_layers )
    {
        if ( debug_messenger_ != VK_NULL_HANDLE )
        {
            destroy_debug_utils_messenger( instance_, debug_messenger_, nullptr );
            debug_messenger_ = VK_NULL_HANDLE;
        }
    }

    if ( instance_ != VK_NULL_HANDLE )
    {
        instance_ = VK_NULL_HANDLE;
    }
}

/**
 * @brief Move Assigmnent Operator.
 */
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
    auto create_info = VkSwapchainCreateInfoKHR{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.surface = surface_;
    
    return create_info;
}

std::variant<VkSwapchainKHR, vk::error::type> context::create_swapchain( vk::swapchain_create_info_t const& create_info ) const
{
    VkSwapchainKHR handle = VK_NULL_HANDLE;
 
    switch ( vkCreateSwapchainKHR( device_, &create_info.value_, nullptr, &handle ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
        case VK_ERROR_DEVICE_LOST:
            return vk::error::type::e_device_lost;
        case VK_ERROR_SURFACE_LOST_KHR:
            return vk::error::type::e_surface_lost;
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return vk::error::type::e_native_window_in_use;
        case VK_ERROR_INITIALIZATION_FAILED:
            return vk::error::type::e_initialization_failed;
        default:
            return handle;
    }
}
void context::destroy_swapchain( vk::swapchain_t swapchain ) const noexcept
{
    vkDestroySwapchainKHR( device_, swapchain.value_, nullptr );
}


std::variant<VkImageView, vk::error::type> context::create_image_view( vk::image_view_create_info_t const& create_info ) const noexcept
{
    VkImageView handle = VK_NULL_HANDLE;

    switch ( vkCreateImageView( device_, &create_info.value_, nullptr, &handle ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
        default:
            return handle;
    }
}
void context::destroy_image_view( vk::image_view_t image_view ) const noexcept
{
    vkDestroyImageView( device_, image_view.value_, nullptr );
}


std::variant<VkRenderPass, vk::error::type> context::create_render_pass( vk::render_pass_create_info_t const& create_info ) const noexcept
{
    VkRenderPass handle = VK_NULL_HANDLE;
    
    switch( vkCreateRenderPass( device_, &create_info.value_, nullptr, &handle ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
        default:
            return handle;
    }
}
void context::destroy_render_pass( vk::render_pass_t render_pass ) const noexcept
{
    vkDestroyRenderPass( device_, render_pass.value_, nullptr );
}

std::variant<VkPipelineLayout, vk::error::type> context::create_pipeline_layout( vk::pipeline_layout_create_info_t const& create_info ) const noexcept
{
    VkPipelineLayout handle = VK_NULL_HANDLE;

    switch( vkCreatePipelineLayout( device_, &create_info.value_, nullptr, &handle ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
        default:
            return handle;
    }
}
void context::destroy_pipeline_layout( vk::pipeline_layout_t pipeline_layout ) const noexcept
{
    vkDestroyPipelineLayout( device_, pipeline_layout.value_, nullptr );
}


std::variant<VkPipeline, vk::error::type> context::create_pipeline( vk::graphics_pipeline_create_info_t const& create_info ) const noexcept
{
    VkPipeline handle = VK_NULL_HANDLE;

    switch( vkCreateGraphicsPipelines( device_, nullptr, 1, &create_info.value_, nullptr, &handle ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
        default:
            return handle;
    }
}
std::variant<VkPipeline, vk::error::type> context::create_pipeline( vk::compute_pipeline_create_info_t const& create_info ) const noexcept
{
    VkPipeline handle = VK_NULL_HANDLE;

    switch( vkCreateComputePipelines( device_, nullptr, 1, &create_info.value_, nullptr, &handle ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
        default:
            return handle;
    }
}
void context::destroy_pipeline( vk::pipeline_t pipeline ) const noexcept
{
     vkDestroyPipeline( device_, pipeline.value_, nullptr );
}


VkShaderModule context::create_shader_module( vk::shader_module_create_info_t const& create_info ) const noexcept
{
    VkShaderModule handle;

    return ( vkCreateShaderModule( device_, &create_info.value_, nullptr, &handle ) == VK_SUCCESS ) ? handle : VK_NULL_HANDLE;
}
void context::destroy_shader_module( vk::shader_module_t shader_module ) const noexcept
{
    vkDestroyShaderModule( device_, shader_module.value_, nullptr );
}


std::variant<VkFramebuffer, vk::error::type> context::create_framebuffer( vk::framebuffer_create_info_t const& create_info ) const noexcept
{
    VkFramebuffer handle = VK_NULL_HANDLE;
    
    switch( vkCreateFramebuffer( device_, &create_info.value_, nullptr, &handle ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
        default:
            return handle;
    }
}
void context::destroy_framebuffer( vk::framebuffer_t framebuffer ) const noexcept
{
    vkDestroyFramebuffer( device_, framebuffer.value_, nullptr ); 
}

std::variant<VkSemaphore, vk::error::type> context::create_semaphore( vk::semaphore_create_info_t const& create_info ) const noexcept
{
    VkSemaphore handle = VK_NULL_HANDLE;

    switch( vkCreateSemaphore( device_, &create_info.value_, nullptr, &handle ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
        default:
            return handle;
    }
}
void context::destroy_semaphore( vk::semaphore_t semaphore ) const noexcept
{
    vkDestroySemaphore( device_, semaphore.value_, nullptr );
}

std::variant<VkFence, vk::error::type> context::create_fence( vk::fence_create_info_t const& create_info ) const noexcept
{
    VkFence handle = VK_NULL_HANDLE;

    switch( vkCreateFence( device_, &create_info.value_, nullptr, &handle ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
        default:
            return handle;
    }
}
void context::destroy_fence( vk::fence_t fence ) const noexcept
{
    vkDestroyFence( device_, fence.value_, nullptr );
}

std::variant<std::vector<VkCommandBuffer>, vk::error::type> context::create_command_buffers( queue::flag_t flag, count32_t buffer_count ) const 
{
    auto pool = command_pools_.find( queues_.find( flag.value_ )->second.get_family_index( ) );

    VkCommandBufferAllocateInfo const allocate_info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = pool->second.handle_,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = buffer_count.value_
    };

    std::vector<VkCommandBuffer> handles( buffer_count.value_ );
    switch( vkAllocateCommandBuffers( device_, &allocate_info, handles.data( ) ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
        default:
            return handles;
    }
}

std::variant<std::vector<VkImage>, vk::error::type> context::get_swapchain_images( vk::swapchain_t swapchain, count32_t image_count ) const
{
    switch ( vkGetSwapchainImagesKHR( device_, swapchain.value_, &image_count.value_, nullptr ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
            break;

        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
            break;

        default:
            break;
    }
    
    std::vector<VkImage> images( image_count.value_ );

    switch ( vkGetSwapchainImagesKHR( device_, swapchain.value_, &image_count.value_, images.data( ) ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
            break;

        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
            break;

        default:
            return images;
            break;
    }
}

VkSurfaceCapabilitiesKHR context::get_surface_capabilities( ) const noexcept
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( gpu_, surface_, &capabilities );
    
    return capabilities;
}

std::vector<std::uint32_t> context::get_unique_family_indices( ) const
{
    std::vector<std::uint32_t> indices;
    indices.reserve( queues_.size() );

    for( auto const& queue : queues_ )
    {
        bool insert = true;
        for( auto index : indices )
        {
            if ( queue.second.get_family_index( ) == index )
            {
                insert = false;
            }
        }

        if ( insert )
        {
            indices.push_back(  queue.second.get_family_index() );
        }
    }

    return indices;
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

vk::error::type context::submit_queue( queue::flag_t flag, vk::submit_info_t const& submit_info, vk::fence_t fence ) const noexcept
{
    if ( auto queue = queues_.find( flag.value_ ); queue != queues_.cend( ) )
    {
        return queue->second.submit( submit_info, fence );
    }
    else
    {
        return vk::error::type::e_invalid_queue;
    }
}
vk::error::type context::present_queue( queue::flag_t flag, vk::present_info_t const& present_info ) const noexcept
{
    if ( auto queue = queues_.find( flag.value_ ); queue != queues_.cend( ) )
    {
        return queue->second.present( present_info );
    }
    else
    {
        return vk::error::type::e_invalid_queue;
    }
}

void context::wait_for_fence( vk::fence_t fence ) const noexcept
{
    vkWaitForFences( device_, 1, &fence.value_, VK_TRUE, std::numeric_limits<std::uint64_t>::max() );
}
void context::reset_fence( vk::fence_t fence ) const noexcept
{
    vkResetFences( device_, 1, &fence.value_ );
}

std::vector<vk::layer> context::load_validation_layers( ) const
{
    if constexpr( vk::enable_debug_layers )
    {
        std::vector<vk::layer> layers =
        {
            vk::layer{ .priority_ = vk::layer::priority::e_optional, .found_ = false, .name_ = "VK_LAYER_KHRONOS_validation" }
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

std::vector<vk::extension> context::load_instance_extensions( ) const
{
    std::vector<vk::extension> exts = 
    {
#if defined( VK_USE_PLATFORM_WIN32_KHR )
        vk::extension{ .priority_ = vk::extension::priority::e_required, .found_ = false, .name_ = "VK_KHR_win32_surface" },
#elif defined( VK_USE_PLATFORM_XCB_KHR )
        vk::extension{ .priority_ = vk::extension::priority::e_required, .found_ = false, .name_ = "VK_KHR_xcb_surface" },
#endif
        vk::extension{ .priority_ = vk::extension::priority::e_required, .found_ = false, .name_ = "VK_KHR_surface" },
        vk::extension{ .priority_ = vk::extension::priority::e_optional, .found_ = false, .name_ = "VK_KHR_load_surface_capabilities2" },
        vk::extension{ .priority_ = vk::extension::priority::e_optional, .found_ = false, .name_ = "VK_EXT_debug_utils" }
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

std::vector<vk::extension> context::load_device_extensions( ) const
{
    std::vector<vk::extension> exts =
    {
        vk::extension{ .priority_ = vk::extension::priority::e_required, .found_ = false, .name_ = "VK_KHR_swapchain" }
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
    enabled_layers.reserve( layers.value_.size( ) );

    for ( const auto& layer : layers.value_ )
    {
        if ( ( !layer.found_ ) && layer.priority_ == vk::layer::priority::e_required )
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
        if ( ( !extension.found_ ) && extension.priority_ == vk::extension::priority::e_required )
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

std::variant<VkInstance, vk::error::type> context::create_instance( 
    const VkApplicationInfo& app_info, 
    const extension_names_t& enabled_ext_name, 
    const layer_names_t& enabled_layer_names ) const
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

    if constexpr ( vk::enable_debug_layers )
    {
        const VkInstanceCreateInfo create_info 
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &app_info,
            .enabledLayerCount = static_cast<uint32_t>( layers.size( ) ),
            .ppEnabledLayerNames = layers.data( ),
            .enabledExtensionCount = static_cast<std::uint32_t>( extensions.size( ) ),
            .ppEnabledExtensionNames = extensions.data( )
        };

        switch( vkCreateInstance( &create_info, nullptr, &handle ) )
        {
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                return vk::error::type::e_out_of_host_memory;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                return vk::error::type::e_out_of_device_memory;
            case VK_ERROR_INITIALIZATION_FAILED:
                return vk::error::type::e_initialization_failed;
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                return vk::error::type::e_incompatible_driver;
            default:
                return handle;
        }  
    }
    else
    {
        const VkInstanceCreateInfo create_info 
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, 
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &app_info,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<std::uint32_t>( extensions.size( ) ),
            .ppEnabledExtensionNames = extensions.data( )
        };

        switch( vkCreateInstance( &create_info, nullptr, &handle ) )
        {
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                return vk::error::type::e_out_of_host_memory;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                return vk::error::type::e_out_of_device_memory;
            case VK_ERROR_INITIALIZATION_FAILED:
                return vk::error::type::e_initialization_failed;
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                return vk::error::type::e_incompatible_driver;
            default:
                return handle;
        }    
    }
}

std::variant<VkDebugUtilsMessengerEXT, vk::error::type> context::create_debug_messenger( ) const
{
    VkDebugUtilsMessengerEXT handle = VK_NULL_HANDLE;

    const VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info 
    {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_callback,
        .pUserData = nullptr
    };

    if ( create_debug_utils_messenger( instance_, &debug_messenger_create_info, nullptr, &handle ) != VK_SUCCESS )
    {
        return vk::error::type::e_out_of_host_memory;
    }
    
    return handle;
}

std::variant<VkSurfaceKHR, vk::error::type> context::create_surface( const ui::window& wnd ) const
{
    return wnd.create_surface( vk::instance_t( instance_ ) );
}

std::variant<VkPhysicalDevice, vk::error::type> context::pick_gpu( ) const
{
    std::uint32_t physical_device_count = 0;
    vkEnumeratePhysicalDevices( instance_, &physical_device_count, nullptr );
    VkPhysicalDevice* physical_devices = reinterpret_cast<VkPhysicalDevice*>( alloca( sizeof( VkPhysicalDevice ) * physical_device_count ) );
    vkEnumeratePhysicalDevices( instance_, &physical_device_count, physical_devices );

    if ( physical_device_count == 0 )
    {
        return vk::error::type::e_no_physical_device_found;
    }

    std::multimap<std::uint32_t, VkPhysicalDevice> candidates;

    for( size_t i = 0; i < physical_device_count; ++i )
    {
        std::uint32_t rating = rate_gpu( vk::physical_device_t( physical_devices[i] ) );
        candidates.insert( { rating, physical_devices[i] } );
    }

    if ( candidates.rbegin( )->first > 0 )
    {
        return candidates.begin( )->second;
    }

    return vk::error::type::e_no_suitable_physical_devices;
}

std::variant<VkDevice, vk::error::type> context::create_device( extension_names_t const& enabled_ext_name, queue_properties_t const& queue_properties ) const
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
                .pNext = nullptr,
                .flags = 0,
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
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = static_cast<std::uint32_t>( queue_create_infos.size( ) ),
        .pQueueCreateInfos = queue_create_infos.data( ),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<std::uint32_t>( extensions.size( ) ),
        .ppEnabledExtensionNames = extensions.data( ),
        .pEnabledFeatures = &features
    };

    switch ( vkCreateDevice( gpu_, &create_info, nullptr, &handle ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
            break;

        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
            break;

        case VK_ERROR_INITIALIZATION_FAILED:
            return vk::error::type::e_initialization_failed;
            break;

        case VK_ERROR_TOO_MANY_OBJECTS:
            return vk::error::type::e_too_many_objects;
            break;

        case VK_ERROR_DEVICE_LOST:
            return vk::error::type::e_device_lost;
            break;

        default:
            return handle;
            break;
    }
}

std::unordered_map<queue::flag, queue> context::get_queues( const queue_properties_t& queue_properties ) const
{
    std::unordered_map<queue::flag, queue> queues;
    queues.reserve( 3 );

    bool has_transfer_only = false;
    bool has_compute_only = false;
    for( size_t i = 0; i < queue_properties.value_.size( ); ++i )
    {
        if ( queue_properties.value_[i].queueCount > 0 )
        {
            if ( queue_properties.value_[i].queueFlags == VK_QUEUE_TRANSFER_BIT )
            {
                has_transfer_only = true;

                queues.insert( { 
                    queue::flag::e_transfer,
                    queue( 
                        vk::device_t( device_ ),  
                        queue::family_index_t( i ), 
                        queue::index_t( 0 ) 
                    ) 
                } );
            }

            if ( queue_properties.value_[i].queueFlags == VK_QUEUE_COMPUTE_BIT )
            {
                has_compute_only = true;
            
                queues.insert( { 
                    queue::flag::e_compute,
                    queue( 
                        vk::device_t( device_ ),  
                        queue::family_index_t( i ), 
                        queue::index_t( 0 ) 
                    ) 
                } );
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

                queues.insert( { 
                    queue::flag::e_graphics,
                    queue( 
                        vk::device_t( device_ ),  
                        queue::family_index_t( i ), 
                        queue::index_t( index ) 
                    ) 
                } );

                ++index;

                if ( !has_transfer_only )
                {
                    queues.insert( { 
                        queue::flag::e_transfer,
                        queue( 
                            vk::device_t( device_ ),  
                            queue::family_index_t( i ), 
                            queue::index_t( index ) 
                        ) 
                    } );

                    ++index;
                }

                if ( !has_compute_only )
                {
                    queues.insert( { 
                        queue::flag::e_compute,
                        queue( 
                            vk::device_t( device_ ),  
                            queue::family_index_t( i ), 
                            queue::index_t( index ) 
                        ) 
                    } );

                    ++index;
                }
            }
        }
    }

    return queues;
}

std::variant<context::command_pools_container_t, vk::error::type> context::create_command_pools( ) const
{
    context::command_pools_container_t command_pools;
    command_pools.reserve( queues_.size( ) );

    for ( const auto& queue : queues_ )
    {
        if ( auto it = command_pools.find( queue.second.get_family_index( ) ); it != command_pools.end( ) )
        {
            it->second.flags_ |= queue.first;
        }
        else
        {
            VkCommandPool handle = VK_NULL_HANDLE;

            VkCommandPoolCreateInfo create_info 
            {
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = queue.second.get_family_index( )
            };

            if ( vkCreateCommandPool( device_, &create_info, nullptr, &handle ) != VK_SUCCESS )
            {
                return vk::error::type::e_command_pool_creation_failed;
            }

            command_pools.insert( { 
                queue.second.get_family_index( ),
                command_pool {
                    .handle_ = handle,
                    .flags_ = queue.first
                }
            } );   
        }
    }

    return command_pools;
}

int context::rate_gpu( vk::physical_device_t const gpu ) const
{
    assert( gpu.value_ != nullptr  && "GPU handle is nullptr.");

    std::uint32_t properties_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( gpu.value_, &properties_count, nullptr );
    VkQueueFamilyProperties* queue_family_properties = reinterpret_cast<VkQueueFamilyProperties*>( alloca( sizeof( VkQueueFamilyProperties ) * properties_count ) );
    vkGetPhysicalDeviceQueueFamilyProperties( gpu.value_, &properties_count, queue_family_properties );

    bool is_rendering_capable = false;
    for( size_t i = 0; i < properties_count; ++i )
    {
        if ( queue_family_properties[i].queueCount > 0 )
        {
            VkBool32 surface_support = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR( gpu.value_, i, surface_, &surface_support );

            if ( ( queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ) && ( surface_support == VK_TRUE ) )
            {
                is_rendering_capable = true;
            }
        }
    }

    if ( !is_rendering_capable )
        return 0;

    std::uint32_t surface_format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR( gpu.value_, surface_, &surface_format_count, nullptr );

    if ( surface_format_count == 0 )
        return 0;

    std::uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR( gpu.value_, surface_, &present_mode_count, nullptr );

    if ( present_mode_count == 0 )
        return 0;

    std::uint32_t score = 0;

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties( gpu.value_, &properties );

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
