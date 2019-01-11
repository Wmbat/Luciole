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

#include <iostream>
#include <set>
#include <map>

#include "renderer.hpp"
#include "../utilities/log.hpp"
#include "vertex.hpp"
#include "../utilities/file_io.hpp"
#include "../utilities/basic_error.hpp"
#include "../utilities/vk_error.hpp"

static constexpr int MAX_FRAMES_IN_FLIGHT = 2;


const std::vector<twe::vertex> vertices = {
    { {  0.0f, -0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
    { {  0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
};

namespace twe
{
    VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback_function( VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj, size_t location,
        int32_t code, const char* layerPrefix,
        const char* msg, void* userData )
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
    
    renderer::renderer( base_window* p_wnd, const std::string& app_name, uint32_t app_version )
        :
        window_width_( p_wnd->get_width() ),
        window_height_( p_wnd->get_height() ),
        clear_colour_( 0.0f, 0.0f, 0.0f, 1.0f )
    {
        p_wnd->set_event_callback( window_close_event_delg( *this, &renderer::on_window_close ) );
        p_wnd->set_event_callback( framebuffer_resize_event_delg( *this, &renderer::on_framebuffer_resize ) );
        
        try
        {
            set_up();
            
            core_info( "Using Vulkan for rendering." );
            
            auto instance = create_instance( app_name, app_version );
            vk_context_.instance_ = std::move( instance );
            
            if constexpr( enable_debug_layers )
            {
                vk_context_.dispatch_loader_dynamic_.init( vk_context_.instance_.get() );
                
                auto debug_callback = create_debug_report_callback();
                vk_context_.debug_report_callback_ = std::move( debug_callback );
            }
            
            vk_context_.surface_ = p_wnd->create_surface( vk_context_.instance_.get() );
            vk_context_.gpu_ = pick_physical_device( );
        
            /* Get GPU info. */
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties( vk_context_.gpu_, &properties );
        
            const auto mem_properties = vk_context_.gpu_.getMemoryProperties( );
            core_info( "Vulkan -> Physical Device picked: {0}", properties.deviceName );
            
            auto device = create_device();
            vk_context_.device_ = std::move( device );
    
            /* Get the graphics queue and the present queue. */
            const auto queue_families = find_queue_family_indices( vk_context_.surface_.get(), vk_context_.gpu_ );
            vk_context_.graphics_queue_ = vk_context_.device_->getQueue( queue_families.graphic_family_.value(), 0 );
            vk_context_.present_queue_ = vk_context_.device_->getQueue( queue_families.present_family_.value(), 0 );
            
            vk_context_.image_available_semaphores_.resize( MAX_FRAMES_IN_FLIGHT );
            vk_context_.render_finished_semaphores_.resize( MAX_FRAMES_IN_FLIGHT );
            vk_context_.in_flight_fences_.resize( MAX_FRAMES_IN_FLIGHT );
            
            for( auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
            {
                auto image_available_semaphore = create_semaphore();
                vk_context_.image_available_semaphores_[i] = std::move( image_available_semaphore );
                
                auto render_finished_semaphore = create_semaphore();
                vk_context_.render_finished_semaphores_[i] = std::move( render_finished_semaphore );
                
                auto fence = create_fence();
                vk_context_.in_flight_fences_[i] = std::move( fence );
            }
            
            auto command_pool = create_command_pool( queue_families.graphic_family_.value() );
            vk_context_.command_pool_ = std::move( command_pool );
            
            const auto swapchain_support_details = query_swapchain_support( vk_context_.surface_.get(), vk_context_.gpu_ );
            const auto present_mode = choose_swapchain_present_mode( swapchain_support_details.present_modes_ );
            const auto surface_format = choose_swapchain_surface_format( swapchain_support_details.formats_ );
            const auto extent = choose_swapchain_extent( swapchain_support_details.capabilities_ );
    
            vk_context_.surface_format_ = surface_format;
            vk_context_.swapchain_.extent_ = extent;
    
            uint32_t image_count = swapchain_support_details.capabilities_.minImageCount + 1;
            if( swapchain_support_details.capabilities_.maxImageCount > 0 &&
                image_count > swapchain_support_details.capabilities_.maxImageCount )
            {
                image_count = swapchain_support_details.capabilities_.maxImageCount;
            }
    
            auto swapchain = create_swapchain( queue_families, present_mode, swapchain_support_details.capabilities_, image_count );
            vk_context_.swapchain_.swapchain_ = std::move( swapchain );
    
            vk_context_.swapchain_.image_ = vk_context_.device_->getSwapchainImagesKHR( vk_context_.swapchain_.swapchain_.get() );
            
            auto render_pass = create_render_pass ( );
            vk_context_.render_pass_ = std::move ( render_pass );

            vk_context_.swapchain_.image_views_.resize( image_count );
            vk_context_.swapchain_.framebuffers_.resize( image_count );
            for ( uint32_t i = 0; i < image_count; ++i )
            {
                auto image_view = create_image_view ( vk_context_.swapchain_.image_[i] );
                vk_context_.swapchain_.image_views_[i] = std::move ( image_view );

                const vk::ImageView attachments[] = {
                    vk_context_.swapchain_.image_views_[i].get ( )
                };

                auto framebuffer = create_framebuffer ( attachments, sizeof ( attachments ) / sizeof ( attachments[0] ) );
                vk_context_.swapchain_.framebuffers_[i] = std::move ( framebuffer );
            }
    
            auto command_buffers = create_command_buffers( image_count );
            vk_context_.command_buffers_ = std::move( command_buffers );
            
            auto mem_allocator_create_info = VmaAllocatorCreateInfo( );
            mem_allocator_create_info.physicalDevice = vk_context_.gpu_;
            mem_allocator_create_info.device = vk_context_.device_.get();
            
            memory_allocator_ = memory_allocator( mem_allocator_create_info );
            
            vertex_buffer_ = vertex_buffer( &vk_context_.device_.get(), vk_context_.gpu_, vertices );
        }
        catch( const basic_error& e )
        {
            core_error( e.what() );
        }
        catch( const vk_error& e )
        {
            core_error( e.what() );
        }
    }
    renderer::renderer( renderer&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    renderer::~renderer( )
    {
        vk_context_.device_->waitIdle( );
        
        if( !vk_context_.command_buffers_.empty() )
        {
            vk_context_.device_->freeCommandBuffers( vk_context_.command_pool_.get(), vk_context_.command_buffers_ );
        }
    }
    
    renderer& renderer::operator=( renderer&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            window_width_ = rhs.window_width_;
            rhs.window_width_ = 0;
            
            window_height_ = rhs.window_height_;
            rhs.window_height_ = 0;
            
            clear_colour_ = std::move( rhs.clear_colour_ );
            
            vk_context_.instance_ = std::move( rhs.vk_context_.instance_ );
        
            if constexpr( enable_debug_layers )
            {
                vk_context_.dispatch_loader_dynamic_ = std::move( rhs.vk_context_.dispatch_loader_dynamic_ );
                
                vk_context_.debug_report_callback_ = std::move( rhs.vk_context_.debug_report_callback_ );
            }
    
            vk_context_.surface_ = std::move( rhs.vk_context_.surface_ );
    
            vk_context_.gpu_ = rhs.vk_context_.gpu_;
            rhs.vk_context_.gpu_ = vk::PhysicalDevice( nullptr );
    
            vk_context_.device_ = std::move( rhs.vk_context_.device_ );
    
            vk_context_.graphics_queue_ = rhs.vk_context_.graphics_queue_;
            rhs.vk_context_.graphics_queue_ = vk::Queue( nullptr );
    
            vk_context_.present_queue_ = rhs.vk_context_.present_queue_;
            rhs.vk_context_.present_queue_ = vk::Queue( nullptr );
            
            vk_context_.in_flight_fences_ = std::move( rhs.vk_context_.in_flight_fences_ );
            vk_context_.image_available_semaphores_ = std::move( rhs.vk_context_.image_available_semaphores_ );
            vk_context_.render_finished_semaphores_ = std::move( rhs.vk_context_.render_finished_semaphores_ );
            
            vk_context_.instance_ = std::move( rhs.vk_context_.instance_ );
    
            vk_context_.command_buffers_ = std::move( rhs.vk_context_.command_buffers_ );
            
            vk_context_.surface_format_ = rhs.vk_context_.surface_format_;
            rhs.vk_context_.surface_format_ = { };
    
            vk_context_.swapchain_.swapchain_ = std::move( rhs.vk_context_.swapchain_.swapchain_ );
            vk_context_.swapchain_.image_ = std::move( rhs.vk_context_.swapchain_.image_ );
            vk_context_.swapchain_.image_views_ = std::move( rhs.vk_context_.swapchain_.image_views_ );
    
            vk_context_.swapchain_.extent_ = rhs.vk_context_.swapchain_.extent_;
            rhs.vk_context_.swapchain_.extent_ = vk::Extent2D( );
    
            vk_context_.render_pass_ = std::move( rhs.vk_context_.render_pass_ );
    
            vk_context_.swapchain_.framebuffers_ = std::move( rhs.vk_context_.swapchain_.framebuffers_ );
            
            vk_context_.instance_extensions_ = std::move( rhs.vk_context_.instance_extensions_ );
            vk_context_.device_extensions_ = std::move( rhs.vk_context_.device_extensions_ );
            vk_context_.validation_layers_ = std::move( rhs.vk_context_.validation_layers_ );
        }
    
        return *this;
    }
    
    void renderer::setup_graphics_pipeline( const shader_data_type &data )
    {
        /*
        const vk::PipelineShaderStageCreateInfo shader_stages[] = {
            shader_manager_.find( data.vert_shader_id_ ).get_shader_stage_create_info(),
            shader_manager_.find( data.frag_shader_id_ ).get_shader_stage_create_info() };
        
        const auto binding_description = vk::VertexInputBindingDescription( )
            .setBinding( 0 )    // vertex buffer
            .setStride( sizeof( vertex ) )
            .setInputRate( vk::VertexInputRate::eVertex );
        
        const auto position_attrib = vk::VertexInputAttributeDescription( )
            .setBinding( data.vertex_position_binding_ )
            .setLocation( data.vertex_position_location_ )
            .setFormat( vk::Format::eR32G32B32Sfloat )
            .setOffset( static_cast<uint32_t>( offsetof( vertex, vertex::position_ ) ) );
        
        const auto colour_attrib = vk::VertexInputAttributeDescription( )
            .setBinding( data.vertex_colour_binding_ )
            .setLocation( data.vertex_colour_location_ )
            .setFormat( vk::Format::eR32G32B32A32Sfloat )
            .setOffset( static_cast<uint32_t>( offsetof( vertex, vertex::colour_ ) ) );
        
        const vk::VertexInputAttributeDescription vertex_input_attribs[] = { position_attrib, colour_attrib };
        
        const auto vertex_input_info = vk::PipelineVertexInputStateCreateInfo( )
            .setVertexBindingDescriptionCount( 0 ) // 1
            .setPVertexBindingDescriptions( &binding_description )
            .setVertexAttributeDescriptionCount( 0 ) // 2
            .setPVertexAttributeDescriptions( vertex_input_attribs );
        
        vk_context_.graphics_pipeline_layout_ = check_vk_result_value(
            create_pipeline_layout( ), "create_pipeline_layout( )" );
        
        vk_context_.graphics_pipeline_ = check_vk_result_value(
            create_graphics_pipeline(
                vertex_input_info,
                sizeof( shader_stages ) / sizeof( shader_stages[0] ),
                shader_stages ),
            "create_graphics_pipeline( )" );
            */
    }
    
    void renderer::record_draw_calls( )
    {
        const auto viewport = vk::Viewport( )
            .setX( 0.0f )
            .setY( 0.0f )
            .setWidth( static_cast<float>( vk_context_.swapchain_.extent_.width ) )
            .setHeight( static_cast<float>( vk_context_.swapchain_.extent_.height ) )
            .setMinDepth( 0.0f )
            .setMaxDepth( 1.0f );
    
        const auto scissors = vk::Rect2D( )
            .setOffset( { 0, 0 } )
            .setExtent( vk_context_.swapchain_.extent_ );
        
        for( size_t i = 0; i < vk_context_.command_buffers_.size(); ++i )
        {
            const auto begin_info = vk::CommandBufferBeginInfo( )
                .setFlags( vk::CommandBufferUsageFlagBits::eSimultaneousUse );
            
            vk_context_.command_buffers_[i].begin( begin_info );
          
            vk_context_.command_buffers_[i].setViewport( 0, 1, &viewport );
            vk_context_.command_buffers_[i].setScissor( 0, 1, &scissors );
            
            const auto clear_colour_value= vk::ClearColorValue( )
                .setFloat32( std::array<float, 4>{ clear_colour_.r / 255.f, clear_colour_.g / 255.f, clear_colour_.b / 255.f, clear_colour_.a / 255.f } );
            
            const auto clear_colour = vk::ClearValue( )
                .setColor( clear_colour_value );
            
            const auto render_pass_begin_info = vk::RenderPassBeginInfo( )
                .setFramebuffer( vk_context_.swapchain_.framebuffers_[i].get() )
                .setRenderPass( vk_context_.render_pass_.get() )
                .setClearValueCount( 1 )
                .setPClearValues( &clear_colour )
                .setRenderArea( { { 0, 0 }, vk_context_.swapchain_.extent_ } );
            
            vk_context_.command_buffers_[i].beginRenderPass( &render_pass_begin_info, vk::SubpassContents::eInline );

            pipeline_manager_.find<graphics_pipeline>( current_pipeline_ ).bind( vk_context_.command_buffers_[i] );
            
            std::array<vk::Buffer, 1> vertex_buffers = { vertex_buffer_.get( ) };
            std::array<vk::DeviceSize, 1> offsets = { 0 };
            vk_context_.command_buffers_[i].bindVertexBuffers( 0, vertex_buffers, offsets );
            
            vk_context_.command_buffers_[i].draw( static_cast<uint32_t>( vertices.size() ), 1, 0, 0 );
    
            vk_context_.command_buffers_[i].endRenderPass( );
            
            vk_context_.command_buffers_[i].end( );
        }
    }
    void renderer::on_window_close ( const window_close_event& event )
    {
        is_window_closed_ = event.is_closed_;
    }
    void renderer::on_framebuffer_resize( const framebuffer_resize_event& event )
    {
        window_width_ = event.size_.x;
        window_height_ = event.size_.y;
        framebuffer_resized_ = true;
    }
    
    void renderer::switch_pipeline( const uint32_t id )
    {
        current_pipeline_ = id;
        
        recreate_swapchain( );
    }
    
    void renderer::draw_frame( )
    {
        if ( !is_window_closed_ )
        {
            vk_context_.device_->waitForFences( 1, &vk_context_.in_flight_fences_[current_frame_].get(),
                VK_TRUE, std::numeric_limits<uint64_t>::max() );

            auto [result, image_index] = vk_context_.device_->acquireNextImageKHR(
                vk_context_.swapchain_.swapchain_.get(),
                std::numeric_limits<uint64_t>::max(),
                vk_context_.image_available_semaphores_[current_frame_].get(), {} );
            
            try
            {
                if ( result == vk::Result::eErrorOutOfDateKHR )
                {
                    recreate_swapchain ( );
                }
                else if ( result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR )
                {
                    throw vk_error{ result, "Failed to acquire swapchain image" };
                }
            }
            catch ( const vk_error& e )
            {
                core_error ( e.what ( ) );
            }

            const vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits ::eColorAttachmentOutput };
            const auto submit_info = vk::SubmitInfo( )
                .setPWaitDstStageMask( wait_stages )
                .setWaitSemaphoreCount( 1 )
                .setPWaitSemaphores( &vk_context_.image_available_semaphores_[current_frame_].get() )
                .setSignalSemaphoreCount( 1 )
                .setPSignalSemaphores( &vk_context_.render_finished_semaphores_[current_frame_].get() )
                .setCommandBufferCount( 1 )
                .setPCommandBuffers( &vk_context_.command_buffers_[image_index] );

            vk_context_.device_->resetFences( 1, &vk_context_.in_flight_fences_[current_frame_].get() );

            try
            {
                check_vk_result( vk_context_.graphics_queue_.submit( 1, &submit_info,
                        vk_context_.in_flight_fences_[current_frame_].get() ),
                    "Failed to submit draw command buffer!" );
            }
            catch ( const vk_error& e )
            {
                core_error ( e.what ( ) );
            }

            /*
            const auto present_info = vk::PresentInfoKHR( )
                .setWaitSemaphoreCount( 1 )
                .setPWaitSemaphores( &vk_context_.render_finished_semaphores_[current_frame_].get() )
                .setSwapchainCount( 1 )
                .setPSwapchains( &vk_context_.swapchain_.swapchain_.get() )
                .setPImageIndices( &image_index );
            
            result = vk_context_.present_queue_.presentKHR( present_info );
            */
            
            auto present_info = VkPresentInfoKHR{ };
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = reinterpret_cast<VkSemaphore*>( &vk_context_.render_finished_semaphores_[current_frame_].get() );
            present_info.swapchainCount = 1;
            present_info.pSwapchains = reinterpret_cast<VkSwapchainKHR*>( &vk_context_.swapchain_.swapchain_.get() );
            present_info.pImageIndices = &image_index;
            
            result = ( vk::Result )vkQueuePresentKHR( vk_context_.present_queue_, &present_info );
            
            try
            {
                if ( result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR ||
                    framebuffer_resized_ )
                {
                    framebuffer_resized_ = false;
                    recreate_swapchain ( );
                }
                else if ( result != vk::Result::eSuccess )
                {
                    throw vk_error{ result, "failed to present swapchain image." };
                }
            }
            catch ( const vk_error& e )
            {
                core_error ( e.what ( ) );
            }

            current_frame_ = ++current_frame_ % MAX_FRAMES_IN_FLIGHT;
        }
    }
    
    void renderer::set_clear_colour( const glm::vec4& colour )
    {
        clear_colour_ = colour;
    }
    
    void renderer::recreate_swapchain( )
    {
        cleanup_swapchain();
        
        const auto queue_family_indices = find_queue_family_indices( vk_context_.surface_.get(), vk_context_.gpu_ );
        const auto swapchain_support_details = query_swapchain_support( vk_context_.surface_.get(), vk_context_.gpu_ );
        const auto present_mode = choose_swapchain_present_mode( swapchain_support_details.present_modes_ );
        const auto surface_format = choose_swapchain_surface_format( swapchain_support_details.formats_ );
        const auto extent = choose_swapchain_extent( swapchain_support_details.capabilities_ );
    
        vk_context_.surface_format_ = surface_format;
        vk_context_.swapchain_.extent_ = extent;
    
        uint32_t image_count = swapchain_support_details.capabilities_.minImageCount + 1;
        if( swapchain_support_details.capabilities_.maxImageCount > 0 &&
            image_count > swapchain_support_details.capabilities_.maxImageCount )
        {
            image_count = swapchain_support_details.capabilities_.maxImageCount;
        }
    
        auto swapchain = create_swapchain( queue_family_indices, present_mode, swapchain_support_details.capabilities_, image_count );
        vk_context_.swapchain_.swapchain_ = std::move( swapchain );
        
        vk_context_.swapchain_.image_ = vk_context_.device_->getSwapchainImagesKHR( vk_context_.swapchain_.swapchain_.get() );
    
        auto render_pass = create_render_pass ( );
        vk_context_.render_pass_ = std::move ( render_pass );

        vk_context_.swapchain_.image_views_.resize( image_count );
        vk_context_.swapchain_.framebuffers_.resize( image_count );
        for( uint32_t i = 0; i < image_count; ++i )
        {
            auto image_view = create_image_view( vk_context_.swapchain_.image_[i] );
            vk_context_.swapchain_.image_views_[i] = std::move( image_view );
            
            const vk::ImageView attachments[] = {
                vk_context_.swapchain_.image_views_[i].get ( )
            };

            auto framebuffer = create_framebuffer( attachments, sizeof( attachments ) / sizeof( attachments[0] ) );
            vk_context_.swapchain_.framebuffers_[i] = std::move( framebuffer );
        }
        
        auto command_buffers = create_command_buffers( image_count );
        vk_context_.command_buffers_ = std::move( command_buffers );
        
        record_draw_calls( );
    }
    void renderer::cleanup_swapchain( )
    {
        vk_context_.device_->waitIdle( );
        
        for ( auto &framebuffer : vk_context_.swapchain_.framebuffers_ )
        {
            auto p = framebuffer.release();
            
            vk_context_.device_->destroyFramebuffer( p );
        }
        {
            auto p = vk_context_.render_pass_.release();
            
            vk_context_.device_->destroyRenderPass( p );
        }
        for ( auto &image_view : vk_context_.swapchain_.image_views_ )
        {
            auto p = image_view.release();
            
            vk_context_.device_->destroyImageView( p );
        }
        {
            auto p = vk_context_.swapchain_.swapchain_.release();
            
            vk_context_.device_->destroySwapchainKHR( p );
        }
        if( !vk_context_.command_buffers_.empty() )
        {
            vk_context_.device_->freeCommandBuffers( vk_context_.command_pool_.get(), vk_context_.command_buffers_ );
        }
    }
    
    
    void renderer::set_up( )
    {
        uint32_t supported_api_version;
        if( vkEnumerateInstanceVersion( &supported_api_version ) != VK_SUCCESS )
        {
            throw basic_error{ basic_error::code::vk_not_supported_error, "Vulkan not Installed" };
        }
        if( supported_api_version != VK_API_VERSION_1_1 )
        {
            throw basic_error{ basic_error::code::vk_version_error, "Vulkan 1.1 not supporetd" };
        }
        
        if constexpr( enable_debug_layers )
        {
            vk_context_.instance_extensions_.emplace_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
        }
        
        vk_context_.instance_extensions_.emplace_back( VK_KHR_SURFACE_EXTENSION_NAME );

#if defined( VK_USE_PLATFORM_WIN32_KHR )
        vk_context_.instance_extensions_.emplace_back( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
        vk_context_.instance_extensions_.emplace_back( VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME );
#elif defined( VK_USE_PLATFORM_XCB_KHR )
        vk_context_.instance_extensions_.emplace_back( VK_KHR_XCB_SURFACE_EXTENSION_NAME );
#endif
        
        vk_context_.validation_layers_.emplace_back( "VK_LAYER_LUNARG_standard_validation" );
        
        vk_context_.device_extensions_.emplace_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );
        
        if( !check_instance_extension_support( vk_context_.instance_extensions_ ) )
        {
            throw basic_error{ basic_error::code::vk_instance_ext_support_error, "Instance extensions requested, but not supporetd" };
        }
        
        if constexpr ( enable_debug_layers )
        {
            if( !check_debug_layer_support( vk_context_.validation_layers_ ) )
            {
                throw basic_error{ basic_error::code::vk_validation_layer_support_error, "VK_LAYER_LUNARG_standard_validation not supported" };
            }
        }
    }
    const vk::UniqueInstance renderer::create_instance( const std::string& app_name, uint32_t app_version ) const noexcept
    {
        const auto app_info = vk::ApplicationInfo( )
            .setApiVersion( VK_API_VERSION_1_1 )
            .setEngineVersion( VK_MAKE_VERSION( 0, 0, 2 ) )
            .setPEngineName( "The Wombat Engine" )
            .setApplicationVersion( app_version )
            .setPApplicationName( app_name.c_str( ) );
    
        if constexpr( enable_debug_layers )
        {
            const auto create_info = vk::InstanceCreateInfo( )
                .setPApplicationInfo( &app_info )
                .setEnabledExtensionCount( static_cast<uint32_t>( vk_context_.instance_extensions_.size( ) ) )
                .setPpEnabledExtensionNames( vk_context_.instance_extensions_.data() )
                .setEnabledLayerCount( static_cast<uint32_t>( vk_context_.validation_layers_.size() ) )
                .setPpEnabledLayerNames( vk_context_.validation_layers_.data() );
        
            return vk::createInstanceUnique( create_info );
        }
        else
        {
            const auto create_info = vk::InstanceCreateInfo( )
                .setPApplicationInfo( &app_info )
                .setEnabledExtensionCount( static_cast<uint32_t>( vk_context_.instance_extensions_.size( ) ) )
                .setPpEnabledExtensionNames( vk_context_.instance_extensions_.data() )
                .setEnabledLayerCount( 0 )
                .setPpEnabledLayerNames( nullptr );
        
            return vk::createInstanceUnique( create_info );
        }
    }
    
    const vk::UniqueHandle<vk::DebugReportCallbackEXT, vk::DispatchLoaderDynamic> renderer::create_debug_report_callback( ) const noexcept
    {
        const auto create_info = vk::DebugReportCallbackCreateInfoEXT( )
            .setFlags( vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning )
            .setPfnCallback( debug_callback_function );
        
        return vk_context_.instance_->createDebugReportCallbackEXTUnique( create_info, nullptr, vk_context_.dispatch_loader_dynamic_ );
    }
    
    const vk::PhysicalDevice renderer::pick_physical_device( ) const noexcept
    {
        std::multimap<uint32_t, vk::PhysicalDevice> candidates;
    
        auto available_devices = vk_context_.instance_->enumeratePhysicalDevices();
        
        for ( auto &physical_device : available_devices )
        {
            if ( is_physical_device_suitable( vk_context_.surface_.get(), physical_device, vk_context_.device_extensions_ ) )
            {
                uint32_t score = 0;
                
                auto properties = physical_device.getProperties( );
                
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
                
                candidates.insert( { score, physical_device } );
            }
        }
        
        if ( candidates.rbegin( )->first > 0 )
        {
            return candidates.rbegin()->second;
        }
        else
        {
            return { };
        }
    }
    
    const vk::UniqueDevice renderer::create_device( ) const noexcept
    {
        const auto queue_families = find_queue_family_indices( vk_context_.surface_.get(), vk_context_.gpu_ );
        std::set<uint32_t> unique_queue_family;
        
        if ( queue_families.graphic_family_.has_value() )
        {
            unique_queue_family.insert( queue_families.graphic_family_.value() );
        }
        
        if( queue_families.compute_family_.has_value() )
        {
            unique_queue_family.insert( queue_families.compute_family_.value() );
        }
        
        if( queue_families.present_family_.has_value() )
        {
            unique_queue_family.insert( queue_families.present_family_.value() );
        }
        
        std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
        queue_create_infos.reserve( unique_queue_family.size() );
        
        const float priority = 1.0f;
        for( const auto& queue_family : unique_queue_family )
        {
            const auto create_info = vk::DeviceQueueCreateInfo( )
                .setPQueuePriorities( &priority )
                .setQueueCount( 1 )
                .setQueueFamilyIndex( queue_family );
            
            queue_create_infos.emplace_back( create_info );
        }
        
        const auto features = vk_context_.gpu_.getFeatures ( );

        if constexpr( enable_debug_layers )
        {
            const auto create_info = vk::DeviceCreateInfo( )
                .setPEnabledFeatures( &features )
                .setQueueCreateInfoCount( static_cast<uint32_t>( queue_create_infos.size() ) )
                .setPQueueCreateInfos( queue_create_infos.data() )
                .setEnabledExtensionCount( static_cast<uint32_t>( vk_context_.device_extensions_.size() ) )
                .setPpEnabledExtensionNames( vk_context_.device_extensions_.data( ) )
                .setEnabledLayerCount( static_cast<uint32_t>( vk_context_.validation_layers_.size( ) ) )
                .setPpEnabledLayerNames( vk_context_.validation_layers_.data() );
            
            return vk_context_.gpu_.createDeviceUnique( create_info );
        }
        else
        {
            const auto create_info = vk::DeviceCreateInfo( )
                .setPEnabledFeatures( &features )
                .setQueueCreateInfoCount( static_cast<uint32_t>( queue_create_infos.size() ) )
                .setPQueueCreateInfos( queue_create_infos.data() )
                .setEnabledExtensionCount( static_cast<uint32_t>( vk_context_.device_extensions_.size() ) )
                .setPpEnabledExtensionNames( vk_context_.device_extensions_.data( ) )
                .setEnabledLayerCount( 0 )
                .setPpEnabledLayerNames( nullptr );
    
            return vk_context_.gpu_.createDeviceUnique( create_info );
        }
    }
    
    const vk::UniqueSemaphore renderer::create_semaphore( ) const noexcept
    {
        const auto create_info = vk::SemaphoreCreateInfo( );
        
        return vk_context_.device_->createSemaphoreUnique( create_info );
    }
    
    const vk::UniqueFence renderer::create_fence( ) const noexcept
    {
        const auto create_info = vk::FenceCreateInfo( )
            .setFlags( vk::FenceCreateFlagBits::eSignaled );
        
        return vk_context_.device_->createFenceUnique( create_info );
    }
    
    const vk::UniqueCommandPool renderer::create_command_pool( uint32_t queue_family ) const noexcept
    {
        const auto create_info = vk::CommandPoolCreateInfo( )
            .setQueueFamilyIndex( queue_family );
        
        return vk_context_.device_->createCommandPoolUnique( create_info );
    }
    
    const std::vector<vk::CommandBuffer> renderer::create_command_buffers( uint32_t count ) const noexcept
    {
        const auto allocate_info = vk::CommandBufferAllocateInfo( )
            .setCommandPool( vk_context_.command_pool_.get() )
            .setCommandBufferCount( count )
            .setLevel( vk::CommandBufferLevel::ePrimary );
        
        return vk_context_.device_->allocateCommandBuffers( allocate_info );
    }

    const vk::UniqueSwapchainKHR renderer::create_swapchain( const queue_family_indices_type& queue_family_indices,
        const vk::PresentModeKHR & present_mode, const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t image_count ) const noexcept
    {
        const uint32_t queue_fam[] = {
            queue_family_indices.graphic_family_.value( ),
            queue_family_indices.present_family_.value( )
        };
    
        if ( queue_family_indices.graphic_family_ !=
             queue_family_indices.present_family_ )
        {
            const auto create_info = vk::SwapchainCreateInfoKHR( )
                .setSurface( vk_context_.surface_.get() )
                .setPresentMode( present_mode )
                .setImageFormat( vk_context_.surface_format_.format )
                .setImageColorSpace( vk_context_.surface_format_.colorSpace )
                .setImageExtent( vk_context_.swapchain_.extent_ )
                .setImageArrayLayers( 1 )
                .setImageUsage( vk::ImageUsageFlagBits::eColorAttachment )
                .setImageSharingMode( vk::SharingMode::eConcurrent )
                .setMinImageCount( image_count )
                .setQueueFamilyIndexCount( 2 )
                .setPQueueFamilyIndices( queue_fam )
                .setPreTransform( capabilities.currentTransform )
                .setCompositeAlpha( vk::CompositeAlphaFlagBitsKHR::eOpaque )
                .setClipped( VK_TRUE );
            
            return vk_context_.device_->createSwapchainKHRUnique( create_info );
        }
        else
        {
            const auto create_info = vk::SwapchainCreateInfoKHR( )
                .setSurface( vk_context_.surface_.get() )
                .setPresentMode( present_mode )
                .setImageFormat( vk_context_.surface_format_.format )
                .setImageColorSpace( vk_context_.surface_format_.colorSpace )
                .setImageExtent( vk_context_.swapchain_.extent_ )
                .setImageArrayLayers( 1 )
                .setImageUsage( vk::ImageUsageFlagBits::eColorAttachment )
                .setImageSharingMode( vk::SharingMode::eExclusive )
                .setMinImageCount( image_count )
                .setQueueFamilyIndexCount( 0 )
                .setPQueueFamilyIndices( nullptr )
                .setPreTransform( capabilities.currentTransform )
                .setCompositeAlpha( vk::CompositeAlphaFlagBitsKHR::eOpaque )
                .setClipped( VK_TRUE );
            
            return vk_context_.device_->createSwapchainKHRUnique( create_info );
        }
    }

    const vk::UniqueImageView renderer::create_image_view( const vk::Image& image ) const noexcept
    {
        const auto mapping = vk::ComponentMapping( )
            .setR( vk::ComponentSwizzle::eIdentity )
            .setG( vk::ComponentSwizzle::eIdentity )
            .setB( vk::ComponentSwizzle::eIdentity )
            .setA( vk::ComponentSwizzle::eIdentity );
        
        const auto subresources_range = vk::ImageSubresourceRange( )
            .setAspectMask( vk::ImageAspectFlagBits::eColor )
            .setBaseMipLevel( 0 )
            .setLevelCount( 1 )
            .setBaseArrayLayer( 0 )
            .setLayerCount( 1 );
        
        const auto create_info = vk::ImageViewCreateInfo( )
            .setFormat( vk_context_.surface_format_.format )
            .setComponents( mapping )
            .setSubresourceRange( subresources_range )
            .setImage( image )
            .setViewType( vk::ImageViewType::e2D );
        
        return vk_context_.device_->createImageViewUnique( create_info );
    }
    
    const vk::UniqueFramebuffer renderer::create_framebuffer( const vk::ImageView* attachments, const std::uint32_t attachment_count ) const noexcept
    {
        const auto create_info = vk::FramebufferCreateInfo( )
            .setRenderPass( vk_context_.render_pass_.get() )
            .setWidth( vk_context_.swapchain_.extent_.width )
            .setHeight( vk_context_.swapchain_.extent_.height )
            .setAttachmentCount( attachment_count )
            .setPAttachments( attachments )
            .setLayers( 1 );
        
        return vk_context_.device_->createFramebufferUnique( create_info );
    }
    
    const vk::UniqueRenderPass renderer::create_render_pass( vk::PipelineBindPoint bind_point ) const noexcept
    {
        const auto colour_attachment = vk::AttachmentDescription( )
            .setFormat( vk_context_.surface_format_.format )
            .setSamples( vk::SampleCountFlagBits::e1 )
            .setLoadOp( vk::AttachmentLoadOp::eClear )
            .setStoreOp( vk::AttachmentStoreOp::eStore )
            .setStencilLoadOp( vk::AttachmentLoadOp::eDontCare )
            .setStencilStoreOp( vk::AttachmentStoreOp::eDontCare )
            .setInitialLayout( vk::ImageLayout::eUndefined )
            .setFinalLayout( vk::ImageLayout::ePresentSrcKHR );
        
        const auto colour_attachment_ref = vk::AttachmentReference( )
            .setAttachment( 0 )
            .setLayout( vk::ImageLayout::eColorAttachmentOptimal );
            
        const auto subpass_description = vk::SubpassDescription( )
            .setPipelineBindPoint( bind_point )
            .setColorAttachmentCount( 1 )
            .setPColorAttachments( &colour_attachment_ref );
        
        const auto dependency = vk::SubpassDependency( )
            .setSrcSubpass( VK_SUBPASS_EXTERNAL )
            .setDstSubpass( 0 )
            .setSrcStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput )
            .setDstStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput )
            .setSrcAccessMask( vk::AccessFlagBits{ } )
            .setDstAccessMask( vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite );
        
        const auto create_info = vk::RenderPassCreateInfo( )
            .setAttachmentCount( 1 )
            .setPAttachments( &colour_attachment )
            .setSubpassCount( 1 )
            .setPSubpasses( &subpass_description )
            .setDependencyCount( 1 )
            .setPDependencies( &dependency );
        
        return vk_context_.device_->createRenderPassUnique( create_info );
    }
    
    bool renderer::check_instance_extension_support( const std::vector<const char*>& instance_extensions ) const noexcept
    {
        uint32_t count;
        vkEnumerateInstanceExtensionProperties( nullptr, &count, nullptr );
        
        std::vector<VkExtensionProperties> available_extensions( count );
        vkEnumerateInstanceExtensionProperties( nullptr, &count, available_extensions.data() );
        
        for( const auto& extension : instance_extensions )
        {
            bool is_supported = false;
            
            for( const auto& extension_property : available_extensions )
            {
                if( strcmp( extension, extension_property.extensionName ) == 0 )
                {
                    is_supported = true;
                    break;
                }
            }
            
            if( !is_supported )
            {
                return false;
            }
        }
        
        return true;
    }
    
    bool renderer::check_debug_layer_support( const std::vector<const char*>& debug_layers ) const noexcept
    {
        std::vector<vk::LayerProperties> available_layers = vk::enumerateInstanceLayerProperties( );
        
        for( const auto& layer : debug_layers )
        {
            bool is_supported = false;
            
            for( const auto& layer_properties : available_layers )
            {
                if( strcmp( layer, layer_properties.layerName ) == 0 )
                {
                    is_supported = true;
                    break;
                }
            }
            
            if( !is_supported )
            {
                return false;
            }
        }
        
        return true;
    }
    
    bool renderer::is_physical_device_suitable( const vk::SurfaceKHR& surface, const vk::PhysicalDevice& physical_device, const std::vector<const char*>& device_extensions ) const noexcept
    {
        return find_queue_family_indices( surface, physical_device ).is_complete() &&
               check_physical_device_extension_support( physical_device, device_extensions ) &&
               is_swapchain_adequate( surface, physical_device );
    }
    
    bool renderer::check_physical_device_extension_support( const vk::PhysicalDevice &physical_device, const std::vector<const char*>& device_extensions ) const noexcept
    {
        std::set<std::string> required_extensions( device_extensions.cbegin(), device_extensions.cend() );
        
        const auto properties = physical_device.enumerateDeviceExtensionProperties( );
        
        for( const auto& property : properties )
        {
            required_extensions.erase( property.extensionName );
        }
        
        return required_extensions.empty();
    }
    
    const renderer::queue_family_indices_type renderer::find_queue_family_indices( const vk::SurfaceKHR& surface, const vk::PhysicalDevice &physical_device ) const noexcept
    {
        queue_family_indices_type indices;
    
        auto queue_properties = physical_device.getQueueFamilyProperties( );
        
        int i = 0;
        for( const auto& queue_property : queue_properties )
        {
            if( queue_property.queueCount > 0 )
            {
                if ( queue_property.queueFlags & vk::QueueFlagBits::eGraphics )
                {
                    indices.graphic_family_ = i;
                }
                if ( queue_property.queueFlags & vk::QueueFlagBits::eCompute )
                {
                    indices.compute_family_ = i;
                }
                
                const auto present_support = physical_device.getSurfaceSupportKHR( i, surface );
                
                if ( present_support )
                {
                    indices.present_family_ = i;
                }
            }
            
            ++i;
        }
        
        return indices;
    }
    
    bool renderer::is_swapchain_adequate( const vk::SurfaceKHR& surface, const vk::PhysicalDevice &physical_device ) const noexcept
    {
        const auto surface_formats = physical_device.getSurfaceFormatsKHR( surface );
        
        const auto present_modes = physical_device.getSurfacePresentModesKHR( surface );
        
        return !surface_formats.empty() && !present_modes.empty();
    }
    
    const renderer::swapchain_support_details_type renderer::query_swapchain_support( const vk::SurfaceKHR& surface,
        const vk::PhysicalDevice &physical_device ) const noexcept
    {
        swapchain_support_details_type details;

        details.capabilities_ = physical_device.getSurfaceCapabilitiesKHR( surface );
        details.formats_ = physical_device.getSurfaceFormatsKHR( surface );
        details.present_modes_ = physical_device.getSurfacePresentModesKHR( surface );

        return details;
    }

    const vk::SurfaceFormatKHR renderer::choose_swapchain_surface_format(
            const std::vector<vk::SurfaceFormatKHR> &available_formats ) const noexcept
    {
        if ( available_formats.size( ) == 1 && available_formats[0].format == vk::Format::eUndefined )
        {
            return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
        }
    
        const auto iter =
            std::find_if( available_formats.cbegin( ), available_formats.cend( ),
                          []( const vk::SurfaceFormatKHR& format )
                          {
                              return format.format == vk::Format::eB8G8R8A8Unorm &&
                                     format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
                          } );
    
        if ( iter != available_formats.cend( ))
        {
            return *iter;
        }
        else
        {
            return available_formats[0];
        }
    }

    const vk::PresentModeKHR renderer::choose_swapchain_present_mode(
            const std::vector<vk::PresentModeKHR> &available_present_modes ) const noexcept
    {
        const auto iter =
            std::find_if( available_present_modes.cbegin( ), available_present_modes.cend( ),
                          []( const vk::PresentModeKHR& present_mode )
                          {
                              return present_mode == vk::PresentModeKHR::eMailbox;
                          } );
    
        if ( iter != available_present_modes.cend( ) )
        {
            return *iter;
        }
        else
        {
            return vk::PresentModeKHR::eFifo;
        }
    }

    const vk::Extent2D renderer::choose_swapchain_extent( const vk::SurfaceCapabilitiesKHR &capabilities ) const noexcept
    {
        if ( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max( ) )
        {
            return capabilities.currentExtent;
        }
        else
        {
            const auto width = std::max( capabilities.minImageExtent.width,
                                         std::min( capabilities.maxImageExtent.width, window_width_ ) );
        
            const auto height = std::max( capabilities.minImageExtent.height,
                                          std::min( capabilities.maxImageExtent.height, window_height_ ) );
        
            return { width, height };
        }
    }
}