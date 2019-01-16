

#ifndef ENGINE_CONTEXT_HPP
#define ENGINE_CONTEXT_HPP

#include <vulkan/vulkan.hpp>
#include <map>

#include "../twe_core.hpp"
#include "../window/base_window.hpp"
#include "../utilities/vk_utils.hpp"

namespace twe
{
    namespace vulkan
    {
        TWE_API VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback_function( VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location,
            int32_t code, const char* layerPrefix, const char* msg, void* userData );
        
        class context
        {
        private:
            struct queue_family_info;
            
        public:
            TWE_API context( ) = default;
    
            TWE_API context( base_window *p_window, const std::string& app_name, uint32_t app_version );
    
            TWE_API context( const context& rhs ) = delete;
    
            TWE_API context( context&& rhs );
    
            TWE_API ~context( ) = default;
    
            TWE_API context& operator=( const context& rhs ) = delete;
    
            TWE_API context& operator=( context&& rhs );

        private:
            using unique_debug_callback = vk::UniqueHandle<vk::DebugReportCallbackEXT, vk::DispatchLoaderDynamic>;
    
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
                
                for ( const auto& info : queue_family_infos )
                {
                    float priority = 1.0f;
                    
                    if( info.is_dedicated_transfer() )
                    {
                        const auto create_info = vk::DeviceQueueCreateInfo( )
                            .setQueueCount( info.count_ )
                            .setPQueuePriorities( &priority )
                            .setQueueFamilyIndex( info.transfer_.value() );
                        
                        queue_create_infos.push_back( create_info );
                    }
                    else if ( info.is_general_purpose() )
                    {
                        const auto create_info = vk::DeviceQueueCreateInfo( )
                            .setQueueCount( info.count_ )
                            .setPQueuePriorities( &priority )
                            .setQueueFamilyIndex( info.graphics_.value() );
                        
                        queue_create_infos.push_back( create_info );
                    }
                }
                
                const auto features = gpu_.getFeatures ( );
    
                if constexpr( enable_debug_layers )
                {
                    const auto create_info = vk::DeviceCreateInfo( )
                        .setPEnabledFeatures( &features )
                        .setQueueCreateInfoCount( static_cast<uint32_t>( queue_create_infos.size() ) )
                        .setPQueueCreateInfos( queue_create_infos.data() )
                        .setEnabledExtensionCount( static_cast<uint32_t>( device_extensions_.size() ) )
                        .setPpEnabledExtensionNames( device_extensions_.data( ) )
                        .setEnabledLayerCount( static_cast<uint32_t>( validation_layers_.size( ) ) )
                        .setPpEnabledLayerNames( validation_layers_.data() );
        
                    return gpu_.createDeviceUnique( create_info );
                }
                else
                {
                    const auto create_info = vk::DeviceCreateInfo( )
                        .setPEnabledFeatures( &features )
                        .setQueueCreateInfoCount( static_cast<uint32_t>( queue_create_infos.size() ) )
                        .setPQueueCreateInfos( queue_create_infos.data() )
                        .setEnabledExtensionCount( static_cast<uint32_t>( device_extensions_.size() ) )
                        .setPpEnabledExtensionNames( device_extensions_.data( ) )
                        .setEnabledLayerCount( 0 )
                        .setPpEnabledLayerNames( nullptr );
        
                    return gpu_.createDeviceUnique( create_info );
                }
            }
            template<class C>
            std::enable_if_t<std::is_same_v<C, vk::UniqueSemaphore>, C> create_handle( ) const noexcept
            {
                const auto create_info = vk::SemaphoreCreateInfo( );
            
                return device_->createSemaphoreUnique( create_info );
            }
            template<class C>
            std::enable_if_t<std::is_same_v<C, vk::UniqueFence>, C> create_handle( ) const noexcept
            {
                const auto create_info = vk::FenceCreateInfo( )
                    .setFlags( vk::FenceCreateFlagBits::eSignaled );
                
                return device_->createFenceUnique( create_info );
            }
            
            bool is_gpu_suitable( const vk::PhysicalDevice& gpu ) const noexcept;
            
            std::vector<queue_family_info> get_queue_family_infos( const std::vector<vk::QueueFamilyProperties>& queue_properties ) const noexcept;
            
            template<class C>
            std::enable_if_t<std::is_same_v<C, vk::Instance>, bool> check_extension_support( const std::vector<vk::ExtensionProperties>& properties ) const noexcept
            {
                for( const auto& inst_extensions : instance_extensions_ )
                {
                    bool is_supported = false;
        
                    for( const auto& property : properties )
                    {
                        if( strcmp( property.extensionName, inst_extensions ) )
                        {
                            is_supported = true;
                        }
                    }
        
                    if( !is_supported )
                    {
                        return false;
                    }
                }
    
                return true;
            }
            template<class C>
            std::enable_if_t<std::is_same_v<C, vk::Device>, bool> check_extension_support( const std::vector<vk::ExtensionProperties>& properties ) const noexcept
            {
                for( const auto& device_extension : device_extensions_ )
                {
                    bool is_supported = false;
        
                    for( const auto& property : properties )
                    {
                        if( strcmp( property.extensionName, device_extension ) )
                        {
                            is_supported = true;
                        }
                    }
        
                    if( !is_supported )
                    {
                        return false;
                    }
                }
    
                return true;
            }
            template<class C>
            std::enable_if_t<std::is_same_v<C, vk::DebugReportCallbackEXT>, bool> check_extension_support( const std::vector<vk::LayerProperties>& properties ) const noexcept
            {
                for( const auto& validation_layer : validation_layers_ )
                {
                    bool is_supported = false;
        
                    for( const auto& property : properties )
                    {
                        if( strcmp( property.layerName, validation_layer ) )
                        {
                            is_supported = true;
                        }
                    }
        
                    if( !is_supported )
                    {
                        return false;
                    }
                }
    
                return true;
            }
        private:
            vk::UniqueInstance instance_;
            
            vk::DispatchLoaderDynamic dispatch_loader_dynamic_;
            unique_debug_callback debug_callback_;
    
            vk::UniqueSurfaceKHR surface_;
            vk::PhysicalDevice gpu_;
            vk::UniqueDevice device_;
            
            vk::Queue graphics_queue_;
            vk::Queue transfer_queue_;
            
            std::vector<vk::UniqueSemaphore> image_available_semaphores_;
            std::vector<vk::UniqueSemaphore> render_finished_semaphores_;
            std::vector<vk::UniqueFence> in_flight_fences_;
            
            std::vector<const char*> instance_extensions_;
            std::vector<const char*> device_extensions_;
            std::vector<const char*> validation_layers_;
            
            
        private:
            struct queue_family_info
            {
                uint32_t count_;
                
                std::optional<uint32_t> graphics_;
                std::optional<uint32_t> present_;
                std::optional<uint32_t> transfer_;
                
                bool has_rendering_support( ) const noexcept
                {
                    return graphics_.has_value() && present_.has_value();
                }
                
                bool is_general_purpose( ) const noexcept
                {
                    return has_rendering_support( ) && present_.has_value() && transfer_.has_value();
                }
                bool is_dedicated_transfer( ) const noexcept
                {
                    return transfer_.has_value() && !graphics_.has_value() && !present_.has_value();
                }
                bool is_dedicated_graphics( ) const noexcept
                {
                    return graphics_.has_value() && !transfer_.has_value();
                }
                
                queue_family_info& set_count( uint32_t count )
                {
                    count_ = count;
                    
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
            };
        };
    }
}

#endif //ENGINE_CONTEXT_HPP
