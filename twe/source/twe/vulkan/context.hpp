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

#ifndef ENGINE_CONTEXT_HPP
#define ENGINE_CONTEXT_HPP

#include <vulkan/vulkan.hpp>
#include <map>

#include "../twe_core.hpp"
#include "../window/base_window.hpp"
#include "utils.hpp"
#include "../utilities/log.hpp"

namespace twe::vulkan
{
    enum class queue_type
    {
        graphics,
        transfer
    };
    
    struct context
    {
        using unique_debug_callback = vk::UniqueHandle<vk::DebugReportCallbackEXT, vk::DispatchLoaderDynamic>;
        
    public:
        struct create_info_type;
        
    private:
        struct queue_family_info;
    
    public:
        context( ) = default;
        
        context( const create_info_type& create_info )
        {
            if ( vk::enumerateInstanceVersion( ) != VK_API_VERSION_1_1)
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
            
            if ( check_extension_support<vk::Instance>( vk::enumerateInstanceExtensionProperties( )))
            {
                // TODO: handle_error
            }
            
            if constexpr ( enable_debug_layers )
            {
                if ( check_extension_support<vk::DebugReportCallbackEXT>( vk::enumerateInstanceLayerProperties( )))
                {
                    // TODO: handle_error
                }
            }
            
            instance_ = create_handle<vk::UniqueInstance>( create_info.app_name_, create_info.app_version_ );
            
            if constexpr ( enable_debug_layers )
            {
                dispatch_loader_dynamic_ = vk::DispatchLoaderDynamic( instance_.get( ));
                debug_callback_ = create_handle<unique_debug_callback>( );
            }
            
            surface_ = create_info.p_wnd_->create_surface( instance_.get( ));
            gpu_ = create_handle<vk::PhysicalDevice>( );
            
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties( gpu_, &properties );
            
            const auto mem_properties = gpu_.getMemoryProperties( );
            core_info( "Vulkan -> Physical Device picked: {0}", properties.deviceName );
            
            const auto queue_family_infos = get_queue_family_infos( gpu_.getQueueFamilyProperties( ));
            
            device_ = create_handle<vk::UniqueDevice>( queue_family_infos );
            
            graphics_command_pools_.resize( create_info.max_frames_in_flight_ );
            
            bool dedicated_transfer = false;
            for ( const auto& info : queue_family_infos )
            {
                if ( info.is_dedicated_transfer( ))
                {
                    dedicated_transfer = true;
                }
            }
            
            for ( const auto& info : queue_family_infos )
            {
                if ( info.is_dedicated_transfer( ))
                {
                    transfer_queue_ = device_->getQueue( info.transfer_.value( ), 0 );
                    
                    transfer_command_pool_ = create_handle<vk::UniqueCommandPool>( info.transfer_.value( ));
                }
                else if ( info.is_general_purpose( ))
                {
                    graphics_queue_ = device_->getQueue( info.graphics_.value( ), 0 );
                    
                    for ( auto& command_pool : graphics_command_pools_ )
                    {
                        command_pool = create_handle<vk::UniqueCommandPool>( info.graphics_.value( ));
                    }
                    
                    if ( !dedicated_transfer )
                    {
                        transfer_queue_ = device_->getQueue( info.transfer_.value( ), 1 );
                        
                        transfer_command_pool_ = create_handle<vk::UniqueCommandPool>( info.transfer_.value( ));
                    }
                }
            }
        }
        
        context( const context& rhs ) = delete;
        
        context( context&& rhs )
        {
            *this = std::move( rhs );
        }
        
        ~context( ) = default;
        
        context& operator=( const context& rhs ) = delete;
        
        context& operator=( context&& rhs )
        {
            if ( this != &rhs )
            {
                instance_ = std::move( rhs.instance_ );
                
                if constexpr ( enable_debug_layers )
                {
                    dispatch_loader_dynamic_ = std::move( rhs.dispatch_loader_dynamic_ );
                    debug_callback_ = std::move( rhs.debug_callback_ );
                }
                
                gpu_ = std::move( rhs.gpu_ );
                device_ = std::move( rhs.device_ );
                surface_ = std::move( rhs.surface_ );
                
                graphics_queue_ = std::move( rhs.graphics_queue_ );
                graphics_command_pools_ = std::move( rhs.graphics_command_pools_ );
                
                transfer_queue_ = std::move( rhs.transfer_queue_ );
                transfer_command_pool_ = std::move( rhs.transfer_command_pool_ );
                
                instance_extensions_ = std::move( rhs.instance_extensions_ );
                validation_layers_ = std::move( rhs.validation_layers_ );
                device_extensions_ = std::move( rhs.device_extensions_ );
            }
            
            return *this;
        }
    
    private:
        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback_function( VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location,
            int32_t code, const char *layerPrefix, const char *msg, void *userData )
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
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, vk::UniqueInstance>, C> create_handle( const std::string& app_name, uint32_t app_version ) const noexcept
        {
            const auto app_info = vk::ApplicationInfo( )
                .setApiVersion(VK_API_VERSION_1_1)
                .setEngineVersion(VK_MAKE_VERSION( 0, 0, 2 ))
                .setPEngineName( "The Wombat Engine" )
                .setApplicationVersion( app_version )
                .setPApplicationName( app_name.c_str( ));
            
            if constexpr( enable_debug_layers )
            {
                const auto create_info = vk::InstanceCreateInfo( )
                    .setPApplicationInfo( &app_info )
                    .setEnabledExtensionCount( static_cast<uint32_t>( instance_extensions_.size( )))
                    .setPpEnabledExtensionNames( instance_extensions_.data( ))
                    .setEnabledLayerCount( static_cast<uint32_t>( validation_layers_.size( )))
                    .setPpEnabledLayerNames( validation_layers_.data( ));
                
                return vk::createInstanceUnique( create_info );
            }
            else
            {
                const auto create_info = vk::InstanceCreateInfo( )
                    .setPApplicationInfo( &app_info )
                    .setEnabledExtensionCount( static_cast<uint32_t>( instance_extensions_.size( )))
                    .setPpEnabledExtensionNames( instance_extensions_.data( ))
                    .setEnabledLayerCount( 0 )
                    .setPpEnabledLayerNames( nullptr );
                
                return vk::createInstanceUnique( create_info );
            }
        }
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, unique_debug_callback>, C> create_handle( ) const noexcept
        {
            const auto create_info = vk::DebugReportCallbackCreateInfoEXT( )
                .setFlags( vk::DebugReportFlagBitsEXT::eError |
                           vk::DebugReportFlagBitsEXT::eWarning |
                           vk::DebugReportFlagBitsEXT::ePerformanceWarning )
                .setPfnCallback( debug_callback_function );
            
            return instance_->createDebugReportCallbackEXTUnique( create_info, nullptr, dispatch_loader_dynamic_ );
        }
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, vk::PhysicalDevice>, C> create_handle( ) const noexcept
        {
            std::multimap<uint32_t, vk::PhysicalDevice> candidates;
            
            auto available_devices = instance_->enumeratePhysicalDevices( );
            
            for ( const auto& gpu : available_devices )
            {
                if ( is_gpu_suitable( gpu ))
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
            
            if ( candidates.rbegin( )->first > 0 )
            {
                return candidates.begin( )->second;
            }
            else
            {
                // TODO: handle error.
            }
        }
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, vk::UniqueDevice>, C> create_handle( const std::vector<queue_family_info>& queue_family_infos ) const noexcept
        {
            std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
            
            std::vector<std::vector<float>> priorities_;
            
            uint32_t j = 0;
            for ( const auto& info : queue_family_infos )
            {
                if ( info.is_dedicated_transfer( ))
                {
                    priorities_.empty( );
                    
                    for ( size_t i = 0; i < info.queue_count_; ++i )
                    {
                        priorities_.emplace_back( 1.0f );
                    }
                    
                    const auto create_info = vk::DeviceQueueCreateInfo( )
                        .setQueueCount( info.queue_count_ )
                        .setPQueuePriorities( priorities_[j].data( ))
                        .setQueueFamilyIndex( info.transfer_.value( ));
                    
                    queue_create_infos.push_back( create_info );
                }
                else if ( info.is_general_purpose( ))
                {
                    priorities_.empty( );
                    
                    for ( size_t i = 0; i < info.queue_count_; ++i )
                    {
                        priorities_.emplace_back( 1.0f );
                    }
                    
                    const auto create_info = vk::DeviceQueueCreateInfo( )
                        .setQueueCount( info.queue_count_ )
                        .setPQueuePriorities( priorities_[j].data( ))
                        .setQueueFamilyIndex( info.graphics_.value( ));
                    
                    queue_create_infos.push_back( create_info );
                }
                
                ++j;
            }
            
            const auto features = gpu_.getFeatures( );
            
            if constexpr( enable_debug_layers )
            {
                const auto create_info = vk::DeviceCreateInfo( )
                    .setPEnabledFeatures( &features )
                    .setQueueCreateInfoCount( static_cast<uint32_t>( queue_create_infos.size( )))
                    .setPQueueCreateInfos( queue_create_infos.data( ))
                    .setEnabledExtensionCount( static_cast<uint32_t>( device_extensions_.size( )))
                    .setPpEnabledExtensionNames( device_extensions_.data( ))
                    .setEnabledLayerCount( static_cast<uint32_t>( validation_layers_.size( )))
                    .setPpEnabledLayerNames( validation_layers_.data( ));
                
                return gpu_.createDeviceUnique( create_info );
            }
            else
            {
                const auto create_info = vk::DeviceCreateInfo( )
                    .setPEnabledFeatures( &features )
                    .setQueueCreateInfoCount( static_cast<uint32_t>( queue_create_infos.size( )))
                    .setPQueueCreateInfos( queue_create_infos.data( ))
                    .setEnabledExtensionCount( static_cast<uint32_t>( device_extensions_.size( )))
                    .setPpEnabledExtensionNames( device_extensions_.data( ))
                    .setEnabledLayerCount( 0 )
                    .setPpEnabledLayerNames( nullptr );
                
                return gpu_.createDeviceUnique( create_info );
            }
        }
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, vk::UniqueCommandPool>, C> create_handle( uint32_t queue_family_index ) const noexcept
        {
            const auto& create_info = vk::CommandPoolCreateInfo( )
                .setQueueFamilyIndex( queue_family_index );
            
            return device_->createCommandPoolUnique( create_info );
        }
        
        bool is_gpu_suitable( const vk::PhysicalDevice& gpu ) const noexcept
        {
            const auto properties = gpu.getQueueFamilyProperties( );
            
            bool is_rendering_capable = false;
            
            int i = 0;
            for ( const auto& property : properties )
            {
                if ( property.queueCount > 0 )
                {
                    if ( property.queueFlags & vk::QueueFlagBits::eGraphics && gpu.getSurfaceSupportKHR( i, surface_.get( )))
                    {
                        is_rendering_capable = true;
                    }
                }
                
                ++i;
            }
            
            const auto surface_formats = gpu.getSurfaceFormatsKHR( surface_.get( ));
            const auto present_modes = gpu.getSurfacePresentModesKHR( surface_.get( ));
            
            return is_rendering_capable && check_extension_support<vk::Device>( gpu.enumerateDeviceExtensionProperties( )) && !surface_formats.empty( ) && !present_modes.empty( );
        }
        
        std::vector<queue_family_info> get_queue_family_infos( const std::vector<vk::QueueFamilyProperties>& queue_properties ) const noexcept
        {
            std::vector<queue_family_info> queue_family_infos;
            
            uint32_t i = 0;
            for ( const auto& queue_property : queue_properties )
            {
                if ( queue_property.queueCount > 0 )
                {
                    if ( queue_property.queueFlags == vk::QueueFlagBits::eTransfer )
                    {
                        const auto info = queue_family_info( )
                            .set_queue_count( queue_property.queueCount )
                            .set_transfer( i );
                        
                        queue_family_infos.push_back( info );
                    }
                    
                    if (( queue_property.queueFlags & vk::QueueFlagBits::eGraphics ) && ( queue_property.queueFlags & vk::QueueFlagBits::eTransfer ) &&
                        ( gpu_.getSurfaceSupportKHR( i, surface_.get( ))))
                    {
                        const auto info = queue_family_info( )
                            .set_queue_count( queue_property.queueCount )
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
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, vk::Instance>, bool> check_extension_support( const std::vector<vk::ExtensionProperties>& properties ) const noexcept
        {
            for ( const auto& inst_extensions : instance_extensions_ )
            {
                bool is_supported = false;
                
                for ( const auto& property : properties )
                {
                    if ( strcmp( property.extensionName, inst_extensions ))
                    {
                        is_supported = true;
                    }
                }
                
                if ( !is_supported )
                {
                    return false;
                }
            }
            
            return true;
        }
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, vk::Device>, bool> check_extension_support( const std::vector<vk::ExtensionProperties>& properties ) const noexcept
        {
            for ( const auto& device_extension : device_extensions_ )
            {
                bool is_supported = false;
                
                for ( const auto& property : properties )
                {
                    if ( strcmp( property.extensionName, device_extension ))
                    {
                        is_supported = true;
                    }
                }
                
                if ( !is_supported )
                {
                    return false;
                }
            }
            
            return true;
        }
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, vk::DebugReportCallbackEXT>, bool> check_extension_support( const std::vector<vk::LayerProperties>& properties ) const noexcept
        {
            for ( const auto& validation_layer : validation_layers_ )
            {
                bool is_supported = false;
                
                for ( const auto& property : properties )
                {
                    if ( strcmp( property.layerName, validation_layer ))
                    {
                        is_supported = true;
                    }
                }
                
                if ( !is_supported )
                {
                    return false;
                }
            }
            
            return true;
        }
    
    public:
        vk::DispatchLoaderDynamic dispatch_loader_dynamic_;
        
        vk::UniqueInstance instance_;
    
        unique_debug_callback debug_callback_;
        
        vk::UniqueSurfaceKHR surface_;
        vk::PhysicalDevice gpu_;
        vk::UniqueDevice device_;
        
        vk::Queue graphics_queue_;
        std::vector<vk::UniqueCommandPool> graphics_command_pools_;
        
        vk::Queue transfer_queue_;
        vk::UniqueCommandPool transfer_command_pool_;
    
    private:
        std::vector<const char *> instance_extensions_;
        std::vector<const char *> device_extensions_;
        std::vector<const char *> validation_layers_;
    
    public:
        struct create_info_type
        {
            create_info_type(
                base_window *p_wnd = nullptr,
                std::string app_name = std::string( ),
                std::uint32_t app_version = 0,
                std::uint32_t max_frames_in_flight = 1 )
                :
                p_wnd_( p_wnd ),
                app_name_( app_name ),
                app_version_( app_version ),
                max_frames_in_flight_( max_frames_in_flight )
            { }
    
            create_info_type& set_window( base_window *p_wnd )
            {
                p_wnd_ = p_wnd;
                return *this;
            }
    
            create_info_type& set_app_name( const std::string& app_name )
            {
                app_name_ = app_name;
                return *this;
            }
    
            create_info_type& set_app_version( const std::uint32_t& app_version )
            {
                app_version_ = app_version;
                return *this;
            }
    
            create_info_type& set_max_frames_in_flight( const std::uint32_t& max_frames_in_flight )
            {
                max_frames_in_flight_ = max_frames_in_flight;
                return *this;
            }
        
            base_window *p_wnd_;
        
            std::string app_name_;
            std::uint32_t app_version_;
            
            std::uint32_t max_frames_in_flight_;
        };
        
    private:
        struct queue_family_info
        {
            queue_family_info(
                const std::uint32_t queue_count = 0,
                const std::optional<std::uint32_t> graphics = std::optional<std::uint32_t>( ),
                const std::optional<std::uint32_t> present = std::optional<std::uint32_t>( ),
                const std::optional<std::uint32_t> transfer = std::optional<std::uint32_t>( ))
                :
                queue_count_( queue_count ),
                graphics_( graphics ),
                present_( present ),
                transfer_( transfer )
            { }
            
            queue_family_info& set_queue_count( uint32_t queue_count )
            {
                queue_count_ = queue_count;
                return *this;
            }
            
            queue_family_info& set_graphics( uint32_t index ) noexcept
            {
                graphics_ = index;
                return *this;
            }
            
            queue_family_info& set_present( uint32_t index ) noexcept
            {
                present_ = index;
                return *this;
            }
            
            queue_family_info& set_transfer( uint32_t index ) noexcept
            {
                transfer_ = index;
                return *this;
            }
            
            bool has_rendering_support( ) const noexcept
            {
                return graphics_.has_value( ) && present_.has_value( );
            }
            
            bool is_general_purpose( ) const noexcept
            {
                return has_rendering_support( ) && present_.has_value( ) && transfer_.has_value( );
            }
            
            bool is_dedicated_transfer( ) const noexcept
            {
                return transfer_.has_value( ) && !graphics_.has_value( ) && !present_.has_value( );
            }
            
            bool is_dedicated_graphics( ) const noexcept
            {
                return graphics_.has_value( ) && !transfer_.has_value( );
            }
            
            uint32_t queue_count_;
            
            std::optional<std::uint32_t> graphics_;
            std::optional<std::uint32_t> present_;
            std::optional<std::uint32_t> transfer_;
        };
    };
}

#endif //ENGINE_CONTEXT_HPP
