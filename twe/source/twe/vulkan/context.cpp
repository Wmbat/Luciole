#include "context.hpp"

#include "../utilities/log.hpp"

namespace twe
{
    namespace vulkan
    {
        VkBool32 debug_callback_function( VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code,
            const char *layerPrefix, const char *msg, void *userData )
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
        
        context::context( base_window *p_window, const std::string& app_name, uint32_t app_version )
        {
            if ( vk::enumerateInstanceVersion( ) != VK_API_VERSION_1_1 )
            {
                // TODO: handle error.
            }
            
            if constexpr ( enable_debug_layers )
            {
                instance_extensions_.emplace_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
    
                validation_layers_.emplace_back( "VK_LAYER_LUNARG_standard_validation" );
            }
            
            instance_extensions_.emplace_back( VK_KHR_SURFACE_EXTENSION_NAME );

#if defined( VK_USE_PLATFORM_WIN32_KHR )
            instance_extensions_.emplace_back( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
            instance_extensions_.emplace_back( VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME );
#elif defined( VK_USE_PLATFORM_XCB_KHR )
            instance_extensions_.emplace_back( VK_KHR_XCB_SURFACE_EXTENSION_NAME );
#endif
    
            device_extensions_.emplace_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );

            if ( check_extension_support<vk::Instance>( vk::enumerateInstanceExtensionProperties( ) ) )
            {
                // TODO: handle_error
            }
            
            if constexpr ( enable_debug_layers )
            {
                if ( check_extension_support<vk::DebugReportCallbackEXT>( vk::enumerateInstanceLayerProperties( ) ) )
                {
                    // TODO: handle_error
                }
            }
            
            instance_ = create_handle<vk::UniqueInstance>( app_name, app_version );
            
            if constexpr ( enable_debug_layers )
            {
                dispatch_loader_dynamic_ = vk::DispatchLoaderDynamic( instance_.get() );
                debug_callback_ = create_handle<unique_debug_callback>( );
            }
            
            surface_ = p_window->create_surface( instance_.get() );
            gpu_ = create_handle<vk::PhysicalDevice>();
            
            const auto queue_family_infos = get_queue_family_infos( gpu_.getQueueFamilyProperties( ) );
            
            device_ = create_handle<vk::UniqueDevice>( queue_family_infos );
            
            bool dedicated_transfer = false;
            for( const auto& info : queue_family_infos )
            {
                if( info.is_dedicated_transfer() )
                {
                    dedicated_transfer = true;
                }
            }
            
            for( const auto& info : queue_family_infos )
            {
                if ( info.is_dedicated_transfer() )
                {
                    transfer_queue_ = device_->getQueue( info.transfer_.value(), 0 );
                }
                else if ( info.is_general_purpose() )
                {
                    graphics_queue_ = device_->getQueue( info.graphics_.value(), 0 );
                    
                    if ( !dedicated_transfer )
                    {
                        transfer_queue_ = device_->getQueue( info.transfer_.value(), 1 );
                    }
                }
            }
            
            
            
        }
        context::context( context&& rhs )
        {
        
        }
    
        context& context::operator=( context&& rhs )
        {
            if ( this != &rhs )
            {
                instance_ = std::move( rhs.instance_ );
                
                
                
                instance_extensions_ = std::move( rhs.instance_extensions_ );
                validation_layers_ = std::move( rhs.validation_layers_ );
                device_extensions_ = std::move( rhs.device_extensions_ );
            }
            
            return *this;
        }
    
        bool context::is_gpu_suitable( const vk::PhysicalDevice& gpu ) const noexcept
        {
            const auto properties = gpu.getQueueFamilyProperties( );

            bool is_rendering_capable = false;
            
            int i = 0;
            for( const auto& property : properties )
            {
                if( property.queueCount > 0 )
                {
                    if ( property.queueFlags & vk::QueueFlagBits::eGraphics && gpu.getSurfaceSupportKHR( i, surface_.get() ) )
                    {
                        is_rendering_capable = true;
                    }
                }
                
                ++i;
            }
    
            const auto surface_formats = gpu.getSurfaceFormatsKHR( surface_.get() );
            const auto present_modes = gpu.getSurfacePresentModesKHR( surface_.get( ) );
            
            return is_rendering_capable && check_extension_support<vk::Device>( gpu.enumerateDeviceExtensionProperties( ) ) && !surface_formats.empty() && !present_modes.empty();
        }
    
        std::vector<context::queue_family_info> context::get_queue_family_infos( const std::vector<vk::QueueFamilyProperties>& queue_properties ) const noexcept
        {
            std::vector<queue_family_info> queue_family_infos;
    
            uint32_t i = 0;
            for( const auto& queue_property : queue_properties )
            {
                if( queue_property.queueCount > 0 )
                {
                    if ( queue_property.queueFlags == vk::QueueFlagBits::eTransfer )
                    {
                        const auto info = queue_family_info( )
                            .set_count( queue_property.queueCount )
                            .set_transfer( i );
                        
                        queue_family_infos.push_back( info );
                    }
            
                    if ( ( queue_property.queueFlags & vk::QueueFlagBits::eGraphics ) && ( queue_property.queueFlags & vk::QueueFlagBits::eTransfer ) &&
                         ( gpu_.getSurfaceSupportKHR( i, surface_.get() ) ) )
                    {
                        const auto info = queue_family_info( )
                            .set_count( queue_property.queueCount )
                            .set_graphics( i )
                            .set_present( i )
                            .set_transfer( i );
                        
                        queue_family_infos.push_back( info );
                    }
                }
        
                ++i;
            }
    
            return queue_family_infos;
        }
    }
}