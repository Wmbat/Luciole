#include <map>

#include "device.hpp"

namespace lcl::vulkan
{
    device::device( const instance& instance, const surface& surface, const std::vector<extension>& desired_extensions )
    {
        std::multimap<uint32_t, vk::PhysicalDevice> candidates;
        
        for ( const auto& gpu : instance.p_instance_->enumeratePhysicalDevices( ) )
        {
            if ( is_gpu_suitable( gpu, surface, desired_extensions ) )
            {
                uint32_t score = 0;
                
                auto properties = gpu.getProperties( );
                
                if ( properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu )
                {
                    score += 2;
                }
                else if ( properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu )
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

        for ( const auto& desired_extension : desired_extensions )
        {
            for ( const auto& extension_property : gpu_.enumerateDeviceExtensionProperties( ) )
            {
                if ( strcmp( desired_extension.name_, extension_property.extensionName ) == 0 )
                {
                    extensions_.enable( desired_extension.name_ );
                }
            }
        }

        const auto enabled_extensions = extensions_.get_enabled_extension_names( );

        for( const char* extension : enabled_extensions )
        {
            core_info( "Device extension \"{}\" enabled.", std::string{ extension } );
        }
    }

    bool device::is_gpu_suitable( const vk::PhysicalDevice& gpu, const surface& surface, const std::vector<extension>& desired_extensions ) const
    {
        const auto extension_properties = gpu.enumerateDeviceExtensionProperties( );

        device_extensions curr_extensions;

        // Check if all the desired extensions are supported by the GPU.
        for ( const auto& desired_extension : desired_extensions )
        {
            for ( const auto& extension_property : extension_properties )
            {
                if ( strcmp( desired_extension.name_, extension_property.extensionName ) == 0 )
                {
                    curr_extensions.enable( desired_extension.name_ );
                }
            }
        }

        // If not all the required extensions are available, print an error message.
        if ( !curr_extensions.is_core_enabled( ) )
        {
            for ( const auto& extension : curr_extensions.extensions_ )
            {
                bool is_supported = false;

                for ( const auto& extension_property : extension_properties )
                {
                    if ( strcmp( extension.name_, extension_property.extensionName ) == 0 )
                    {
                        is_supported = true;
                    }
                }

                if ( is_supported )
                {
                    if ( extension.mode_ == extension_mode::e_required && extension.enabled_ == false )
                    {
                        core_warn( "Device extension \"{}\" missing, it is required for the library to function properly. "
                        "Please add the extension to the list of desired extensions.", std::string{ extension.name_ } );
                    }
                }
                else
                {
                    core_error( "Device extension \"{}\" is not supported by the GPU", std::string{ extension.name_ } );
                }
            }

            return false;
        }

        for ( const auto desired_extension : desired_extensions )
        {
            bool is_supported = false;

            for ( const auto& extension : curr_extensions.get_enabled_extension_names( ) )
            {
                if ( strcmp( extension, desired_extension.name_ ) == 0 )
                {
                    is_supported = true;
                }
            }

            if ( !is_supported )
            {
                if ( desired_extension.mode_ == extension_mode::e_required )
                {
                    return false;
                }
            }
        }

        bool is_rendering_capable = false;
        
        int i = 0;
        for ( const auto& property : gpu.getQueueFamilyProperties( ) )
        {
            if ( property.queueCount > 0 )
            {
                auto test = surface.get( );
                if ( property.queueFlags & vk::QueueFlagBits::eGraphics && gpu.getSurfaceSupportKHR( i, surface.get( )))
                {
                    is_rendering_capable = true;
                }
            }
            
            ++i;
        }
        
        const auto surface_formats = gpu.getSurfaceFormatsKHR( surface.get( ));
        const auto present_modes = gpu.getSurfacePresentModesKHR( surface.get( ));

        return is_rendering_capable && !surface_formats.empty( ) && !present_modes.empty( );
    }
}
