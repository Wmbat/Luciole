#include <map>

#include "device.hpp"

namespace lcl::vulkan
{
    device::device( const instance& instance, const surface& surface )
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
    }

    bool device::is_gpu_suitable( const  VkPhysicalDevice& gpu, const surface& surface ) const
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

        // Check if all desired extensions are supported.
        /*
        for ( const auto& desired_extension : desired_extensions )
        {
            bool is_supported = false;

            for ( const auto& enabled_extension : curr_extensions.get_enabled_extension_names( ) )
            {
                if ( strcmp( desired_extension.name_, enabled_extension ) == 0 )
                {
                    is_supported = true;
                }
            }

            if ( !is_supported && desired_extension.mode_ == extension_mode::e_required )
            {
                return false;
            }
        }
        */
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
}
