#include "instance.hpp"

#include "utilities/log.hpp"

#include "vulkan/utils.hpp"

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

    instance::instance( const std::vector<const char*>& desired_extensions )
    {            
        const std::uint32_t api_version = volkGetInstanceVersion( );

        core_info( "Using Vulkan {}.{}.{}", VK_VERSION_MAJOR( api_version ), VK_VERSION_MINOR( api_version ), VK_VERSION_PATCH( api_version ) );

        std::vector<const char*> core_extensions;
        std::vector<const char*> validation_layers;

        if constexpr ( vulkan::enable_debug_layers )
        {
            core_extensions.emplace_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
            validation_layers.emplace_back( "VK_LAYER_LUNARG_standard_validation" );
        }
            
        core_extensions.emplace_back( VK_KHR_SURFACE_EXTENSION_NAME );

#if defined( VK_USE_PLATFORM_WIN32_KHR )
        core_extensions.emplace_back( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
        core_extensions.emplace_back( VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME );
#elif defined( VK_USE_PLATFORM_XCB_KHR )
        core_extensions.emplace_back( VK_KHR_XCB_SURFACE_EXTENSION_NAME );
#endif

        check_extension_support( core_extensions );
        check_layer_support( validation_layers );

        const auto enabled_extensions = extensions_.get_enabled_extension_names( );
        const auto enabled_layers = layers_.get_enabled_layers( );
        
        for( const char* extension : enabled_extensions )
        {
            core_info( "Extension \"{}\" enabled.", std::string{ extension } );
        }

        for( const char* layer : enabled_layers )
        {
            core_info( "Layer \"{}\" enabled.", std::string{ layer } );
        }

        const auto app_info = vk::ApplicationInfo{ }
            .setApiVersion( api_version )
            .setEngineVersion( VK_MAKE_VERSION( 0, 0, 6 ) )
            .setPEngineName( "Luciole" );

        if constexpr( enable_debug_layers )
        {
            const auto create_info = vk::InstanceCreateInfo( )
                .setPApplicationInfo( &app_info )
                .setEnabledExtensionCount( static_cast<uint32_t>( enabled_extensions.size( )))
                .setPpEnabledExtensionNames( enabled_extensions.data( ) )
                .setEnabledLayerCount( static_cast<uint32_t>( enabled_layers.size( ) ) )
                .setPpEnabledLayerNames( enabled_layers.data( ) );
            
            p_instance_ = vk::createInstanceUnique( create_info );
        }
        else
        {
            const auto create_info = vk::InstanceCreateInfo( )
                .setPApplicationInfo( &app_info )
                .setEnabledExtensionCount( static_cast<uint32_t>( enabled_extensions.size( )))
                .setPpEnabledExtensionNames( enabled_extensions.data( ))
                .setEnabledLayerCount( 0 )
                .setPpEnabledLayerNames( nullptr );
            
            p_instance_ = vk::createInstanceUnique( create_info );
        }

        volkLoadInstance( p_instance_.get() );

        if constexpr ( enable_debug_layers )
        {
            const auto create_info = vk::DebugReportCallbackCreateInfoEXT( )
                .setFlags( vk::DebugReportFlagBitsEXT::eError |
                    vk::DebugReportFlagBitsEXT::eWarning |
                    vk::DebugReportFlagBitsEXT::ePerformanceWarning )
                .setPfnCallback( debug_callback_function );
            
            debug_report_ = p_instance_->createDebugReportCallbackEXTUnique( create_info );
        }
    }

    const vk::Instance& instance::get( ) const noexcept
    {
        return p_instance_.get( );
    }

    vk::Instance& instance::get( ) noexcept
    {
        return p_instance_.get( );
    }

    void instance::check_extension_support( const std::vector<const char*>& desired_extensions )
    {
        const auto supported_extensions_properties = vk::enumerateInstanceExtensionProperties( );
        for( const char* extension : desired_extensions )
        {
            for( const auto& properties : supported_extensions_properties )
            {
                if ( strcmp( properties.extensionName, extension ) == 0 )
                {
                    extensions_.enable( extension );
                }
            }
        }
    }

    void instance::check_layer_support( const std::vector<const char*>& desired_layers )
    {
        const auto supported_layer_properties = vk::enumerateInstanceLayerProperties( );
        for( const char* layer : desired_layers )
        {
            for ( const auto& properties : supported_layer_properties )
            {
                if ( strcmp( properties.layerName, layer ) == 0 )
                {
                    layers_.enable( layer );
                }
            }
        }
    }
}