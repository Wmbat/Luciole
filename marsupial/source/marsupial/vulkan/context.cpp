#include "context.hpp"

namespace marsupial::vulkan
{
    /*!
     * Implementation of context::create_info struct.
     */
    context::create_info::create_info( 
        base_window *p_wnd,
        std::string_view app_name,
        std::uint32_t app_version,
        std::uint32_t max_frames_in_flight ) noexcept
        :
        p_wnd_( p_wnd ),
        app_name_( app_name ),
        app_version_( app_version ),
        max_frames_in_flight_( max_frames_in_flight )
    { }

    context::create_info& context::create_info::set_p_window( base_window* p_wnd ) noexcept
    {
        p_wnd_ = p_wnd;
        return *this;
    }
    context::create_info& context::create_info::set_app_name( std::string_view app_name ) noexcept            
    {
        app_name_ = app_name;
        return *this;
    }
    context::create_info& context::create_info::set_app_version( std::uint32_t version ) noexcept
    {
        app_version_ = version;
        return *this;
    }
    context::create_info& context::create_info::set_max_frames_in_flight( std::uint32_t count ) noexcept
    {
        max_frames_in_flight_ = count;
        return *this;
    }

    /*!
     * Implementation of context::queue_family_info struct.
     */
    context::queue_family_info::queue_family_info( 
        const std::uint32_t queue_count,
        const std::optional<std::uint32_t> graphics,
        const std::optional<std::uint32_t> present,
        const std::optional<std::uint32_t> transfer )
        :
        queue_count_( queue_count ),
        graphics_( graphics ),
        present_( present ),
        transfer_( transfer )
    { }

    context::queue_family_info& context::queue_family_info::set_queue_count( std::uint32_t queue_count ) noexcept
    {
        queue_count_ = queue_count;
        return *this;
    }
    context::queue_family_info& context::queue_family_info::set_graphics( std::uint32_t index ) noexcept
    {
        graphics_ = index;
        return *this;
    }
    context::queue_family_info& context::queue_family_info::set_present( std::uint32_t index ) noexcept
    {
        present_ = index;
        return *this;
    }
    context::queue_family_info& context::queue_family_info::set_transfer( std::uint32_t index ) noexcept
    {
        transfer_ = index;
        return *this;
    }

    bool context::queue_family_info::has_rendering_support( ) const noexcept
    {
        return graphics_.has_value( ) && present_.has_value( );
    }
    bool context::queue_family_info::is_general_purpose( ) const noexcept
    {
        return has_rendering_support( ) && present_.has_value( ) && transfer_.has_value( );
    }
    bool context::queue_family_info::is_dedicated_transfer( ) const noexcept
    {
        return transfer_.has_value( ) && !graphics_.has_value( ) && !present_.has_value( );
    }
    bool context::queue_family_info::is_dedicated_graphics( ) const noexcept
    {
        return graphics_.has_value( ) && !transfer_.has_value( );
    }


    /*!
     *
     */
    template<>
    bool context::check_extension_support<vk::Instance>( const extension_properties_t<vk::Instance>& properties ) const
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

    template<>
    bool context::check_extension_support<vk::Device>( const extension_properties_t<vk::Device>& properties ) const
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

    template<>
    bool context::check_extension_support<vk::DebugReportCallbackEXT>( const extension_properties_t<vk::DebugReportCallbackEXT>& properties ) const
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


    /*!
     *
     */
    context::context( const create_info& create_info )
    {
        if( auto result = volkInitialize( ); result != VK_SUCCESS )
        {
                // HANDLE ERROR.
        }
            
        auto api_version = volkGetInstanceVersion( );
            
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
            if ( check_extension_support<vk::DebugReportCallbackEXT>( vk::enumerateInstanceLayerProperties( )))
            {
                    // TODO: handle_error
            }
        }
            
            
        instance_ = create_instance( api_version, create_info.app_name_, create_info.app_version_ );
            
        // Load the functions used by the instance.
        volkLoadInstance( instance_.get() );

        if constexpr ( enable_debug_layers )
        {
            debug_callback_ = create_debug_report_callback( );
        }
            
        surface_ = create_info.p_wnd_->create_surface( instance_.get( ));
        gpu_ = pick_physical_device( );
            
        /* Get the information of all the queue families. */
        const auto queue_family_infos = get_queue_family_infos( gpu_.getQueueFamilyProperties( ));
            
        device_ = create_logical_device( queue_family_infos );
            
        /* Load all functions used from VkDevice */
        volkLoadDevice( device_.get() );
            
        /* Prepare one VkCommandPool for graphics queues per frame. */
        graphics_command_pools_.resize( create_info.max_frames_in_flight_ );
            
        /* Check if a queue family has support for a transfer queue */
        bool dedicated_transfer = false;
        for ( const auto& info : queue_family_infos )
        {
            if ( info.is_dedicated_transfer( ))
            {
                dedicated_transfer = true;
            }
        }
        
        /* Create VkCommandPools for all necessary queue families (graphics and transfer) */
        for ( const auto& info : queue_family_infos )
        {
            if ( info.is_dedicated_transfer( ))
            {
                ++queue_family_count_;
                
                queue_family_indices_.emplace_back( info.transfer_.value( ) );
                
                transfer_queue_ = device_->getQueue( info.transfer_.value( ), 0 );
                
                transfer_command_pool_ = create_command_pool( info.transfer_.value( ));
            }
            else if ( info.is_general_purpose( ))
            {
                ++queue_family_count_;
                
                queue_family_indices_.emplace_back( info.transfer_.value( ) );
                
                graphics_queue_ = device_->getQueue( info.graphics_.value( ), 0 );
                
                for ( auto& command_pool : graphics_command_pools_ )
                {
                    command_pool = create_command_pool( info.graphics_.value( ));
                }
                
                if ( !dedicated_transfer )
                {
                    // TODO: check for IGPU
                    transfer_queue_ = device_->getQueue( info.transfer_.value( ), 1 );
                    
                    transfer_command_pool_ = create_command_pool( info.transfer_.value( ));
                }
            }
        }
    }
    context::context( context&& rhs )
    {
        *this = std::move( rhs );
    }

    context& context::operator=( context&& rhs )
    {
        if ( this != &rhs )
        {
            instance_ = std::move( rhs.instance_ );
            
            if constexpr ( enable_debug_layers )
            {
                debug_callback_ = std::move( rhs.debug_callback_ );
            }
            
            gpu_ = std::move( rhs.gpu_ );
            device_ = std::move( rhs.device_ );
            surface_ = std::move( rhs.surface_ );
            
            graphics_queue_ = std::move( rhs.graphics_queue_ );
            transfer_queue_ = std::move( rhs.transfer_queue_ );
            
            queue_family_count_ = rhs.queue_family_count_;
            rhs.queue_family_count_ = 0;
            
            queue_family_indices_ = std::move( rhs.queue_family_indices_ );
            
            graphics_command_pools_ = std::move( rhs.graphics_command_pools_ );
            transfer_command_pool_ = std::move( rhs.transfer_command_pool_ );
            
            instance_extensions_ = std::move( rhs.instance_extensions_ );
            validation_layers_ = std::move( rhs.validation_layers_ );
            device_extensions_ = std::move( rhs.device_extensions_ );
        }
        
        return *this;
    }

    vk::UniqueInstance context::create_instance( const std::uint32_t api_version, const std::string_view app_name, const std::uint32_t app_version ) const noexcept
    {
        const auto app_info = vk::ApplicationInfo( )
            .setApiVersion( api_version )
            .setEngineVersion(VK_MAKE_VERSION( 0, 0, 2 ))
            .setPEngineName( "The Wombat Engine" )
            .setApplicationVersion( app_version )
            .setPApplicationName( app_name.data( ) );
        
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
    vk::UniqueDebugReportCallbackEXT context::create_debug_report_callback( ) const noexcept
    {
        const auto create_info = vk::DebugReportCallbackCreateInfoEXT( )
            .setFlags( vk::DebugReportFlagBitsEXT::eError |
                vk::DebugReportFlagBitsEXT::eWarning |
                vk::DebugReportFlagBitsEXT::ePerformanceWarning )
            .setPfnCallback( debug_callback_function );
            
        return instance_->createDebugReportCallbackEXTUnique( create_info );
    }
    vk::PhysicalDevice context::pick_physical_device( ) const
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
    vk::UniqueDevice context::create_logical_device( const std::vector<queue_family_info>& queue_family_infos ) const
    {
        std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
        
        std::vector<std::vector<float>> priorities_;
        
        uint32_t j = 0;
        for ( const auto& info : queue_family_infos )
        {
            if ( info.is_dedicated_transfer( ))
            {
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
    vk::UniqueCommandPool context::create_command_pool( uint32_t queue_family_index ) const noexcept
    {
        const auto& create_info = vk::CommandPoolCreateInfo( )
            .setQueueFamilyIndex( queue_family_index );
        
        return device_->createCommandPoolUnique( create_info );
    }

    bool context::is_gpu_suitable( const vk::PhysicalDevice gpu ) const
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
        
        return is_rendering_capable && check_extension_support<vk::Device>( gpu.enumerateDeviceExtensionProperties( ) ) && !surface_formats.empty( ) && !present_modes.empty( );
    }

    std::vector<context::queue_family_info> context::get_queue_family_infos( const std::vector<vk::QueueFamilyProperties>& properties ) const
    {
        std::vector<queue_family_info> queue_family_infos;
        
        uint32_t i = 0;
        for ( const auto& queue_property : properties )
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
}