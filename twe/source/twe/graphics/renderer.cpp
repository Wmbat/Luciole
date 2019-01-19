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
#include "vertex.hpp"
#include "../utilities/log.hpp"
#include "../utilities/file_io.hpp"
#include "../utilities/basic_error.hpp"
#include "../utilities/vk_error.hpp"

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
        
        const auto context_create_info = vulkan::context::create_info_type( )
            .set_window( p_wnd )
            .set_app_name( app_name )
            .set_app_version( app_version )
            .set_max_frames_in_flight( MAX_FRAMES_IN_FLIGHT );
        
        context_ = vulkan::context( context_create_info );
    
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
        
        try
        {
            // TO something about that //
            const auto swapchain_support_details = query_swapchain_support( context_.surface_.get(), context_.gpu_ );
            const auto surface_format = choose_swapchain_surface_format( swapchain_support_details.formats_ );
    
            vk_context_.surface_format_ = surface_format;
            ///
            
            uint32_t image_count = swapchain_support_details.capabilities_.minImageCount + 1;
            if( swapchain_support_details.capabilities_.maxImageCount > 0 &&
                image_count > swapchain_support_details.capabilities_.maxImageCount )
            {
                image_count = swapchain_support_details.capabilities_.maxImageCount;
            }
    
            auto render_pass = create_render_pass ( );
            vk_context_.render_pass_ = std::move ( render_pass );
    
            const auto swapchain_create_info = vulkan::swapchain::create_info_type( )
                .set_gpu( context_.gpu_ )
                .set_device( context_.device_.get() )
                .set_surface( context_.surface_.get() )
                .set_render_pass( vk_context_.render_pass_.get() )
                .set_width( window_width_ )
                .set_height( window_height_ );
    
            swapchain_ = vulkan::swapchain( swapchain_create_info );
            
            
            auto command_buffers = create_command_buffers( swapchain_.image_count_ );
            vk_context_.command_buffers_ = std::move( command_buffers );
            
            auto mem_allocator_create_info = VmaAllocatorCreateInfo( );
            mem_allocator_create_info.physicalDevice = context_.gpu_;
            mem_allocator_create_info.device = context_.device_.get();
            
            memory_allocator_ = vulkan::memory_allocator( mem_allocator_create_info );
            
            vertex_buffer_ = vertex_buffer( memory_allocator_, vertices );
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
        context_.device_->waitIdle( );
        
        if( !vk_context_.command_buffers_.empty() )
        {
            context_.device_->freeCommandBuffers( context_.graphics_command_pools_[0].get(), vk_context_.command_buffers_ );
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
            
            vk_context_.in_flight_fences_ = std::move( rhs.vk_context_.in_flight_fences_ );
            vk_context_.image_available_semaphores_ = std::move( rhs.vk_context_.image_available_semaphores_ );
            vk_context_.render_finished_semaphores_ = std::move( rhs.vk_context_.render_finished_semaphores_ );
            
            vk_context_.command_buffers_ = std::move( rhs.vk_context_.command_buffers_ );
            
            vk_context_.surface_format_ = rhs.vk_context_.surface_format_;
            rhs.vk_context_.surface_format_ = { };
            
            vk_context_.render_pass_ = std::move( rhs.vk_context_.render_pass_ );
            
            vk_context_.instance_extensions_ = std::move( rhs.vk_context_.instance_extensions_ );
            vk_context_.device_extensions_ = std::move( rhs.vk_context_.device_extensions_ );
            vk_context_.validation_layers_ = std::move( rhs.vk_context_.validation_layers_ );
        }
    
        return *this;
    }
    
    void renderer::record_draw_calls( )
    {
        std::vector<vk::Viewport> viewports;
        std::vector<vk::Rect2D> scissors;
        
        const auto viewport = vk::Viewport( )
            .setX( 0.0f )
            .setY( 0.0f )
            .setWidth( static_cast<float>( swapchain_.extent_.width ) )
            .setHeight( static_cast<float>( swapchain_.extent_.height ) )
            .setMinDepth( 0.0f )
            .setMaxDepth( 1.0f );
        
        viewports.push_back( viewport );
    
        const auto scissor = vk::Rect2D( )
            .setOffset( { 0, 0 } )
            .setExtent( swapchain_.extent_ );
        
        scissors.push_back( scissor );
        
        for( size_t i = 0; i < vk_context_.command_buffers_.size(); ++i )
        {
            const auto begin_info = vk::CommandBufferBeginInfo( )
                .setFlags( vk::CommandBufferUsageFlagBits::eSimultaneousUse );
            
            vk_context_.command_buffers_[i].begin( begin_info );
          
            
            const auto clear_colour_value= vk::ClearColorValue( )
                .setFloat32( std::array<float, 4>{ clear_colour_.r / 255.f, clear_colour_.g / 255.f, clear_colour_.b / 255.f, clear_colour_.a / 255.f } );
            
            const auto clear_colour = vk::ClearValue( )
                .setColor( clear_colour_value );
            
            const auto render_pass_begin_info = vk::RenderPassBeginInfo( )
                .setFramebuffer( swapchain_.framebuffers_[i].get() )
                .setRenderPass( vk_context_.render_pass_.get() )
                .setClearValueCount( 1 )
                .setPClearValues( &clear_colour )
                .setRenderArea( { { 0, 0 }, swapchain_.extent_ } );
            
            vk_context_.command_buffers_[i].beginRenderPass( &render_pass_begin_info, vk::SubpassContents::eInline );
            
            const auto& pipeline = pipeline_manager_.find<pipeline_type::graphics>( current_pipeline_ );
            
            pipeline.set_dynamic_state<dynamic_state_type::viewport>( vk_context_.command_buffers_[i], 0, viewports );
            pipeline.set_dynamic_state<dynamic_state_type::scissor>( vk_context_.command_buffers_[i], 0, scissors );
            
            pipeline.bind( vk_context_.command_buffers_[i] );
            
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
    
    void renderer::set_pipeline( const uint32_t id )
    {
        current_pipeline_ = id;
    
        const auto swapchain_create_info = vulkan::swapchain::create_info_type( )
            .set_gpu( context_.gpu_ )
            .set_device( context_.device_.get() )
            .set_surface( context_.surface_.get() )
            .set_render_pass( vk_context_.render_pass_.get() )
            .set_width( window_width_ )
            .set_height( window_height_ );
        
        swapchain_.recreate( swapchain_create_info );
    
        context_.device_->resetCommandPool( context_.graphics_command_pools_[0].get( ), { } );
    
        record_draw_calls( );
    }
    void renderer::switch_pipeline( const uint32_t id )
    {
    }
    
    void renderer::draw_frame( )
    {
        if ( !is_window_closed_ )
        {
            context_.device_->waitForFences( 1, &vk_context_.in_flight_fences_[current_frame_].get(),
                VK_TRUE, std::numeric_limits<uint64_t>::max() );

            auto [result, image_index] = context_.device_->acquireNextImageKHR(
                swapchain_.swapchain_.get(),
                std::numeric_limits<uint64_t>::max(),
                vk_context_.image_available_semaphores_[current_frame_].get(), {} );
            
            try
            {
                if ( result == vk::Result::eErrorOutOfDateKHR )
                {
                    const auto swapchain_create_info = vulkan::swapchain::create_info_type( )
                        .set_gpu( context_.gpu_ )
                        .set_device( context_.device_.get() )
                        .set_surface( context_.surface_.get() )
                        .set_render_pass( vk_context_.render_pass_.get() )
                        .set_width( window_width_ )
                        .set_height( window_height_ );
    
                    swapchain_.recreate( swapchain_create_info );
    
                    context_.device_->resetCommandPool( context_.graphics_command_pools_[0].get( ), { } );
    
                    record_draw_calls( );
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

            context_.device_->resetFences( 1, &vk_context_.in_flight_fences_[current_frame_].get() );

            try
            {
                context_.graphics_queue_.submit( 1, &submit_info, vk_context_.in_flight_fences_[current_frame_].get()  );
            }
            catch ( const vk_error& e )
            {
                core_error ( e.what ( ) );
            }
            
            auto present_info = VkPresentInfoKHR{ };
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = reinterpret_cast<VkSemaphore*>( &vk_context_.render_finished_semaphores_[current_frame_].get() );
            present_info.swapchainCount = 1;
            present_info.pSwapchains = reinterpret_cast<VkSwapchainKHR*>( &swapchain_.swapchain_.get() );
            present_info.pImageIndices = &image_index;
            
            result = ( vk::Result )vkQueuePresentKHR( context_.graphics_queue_, &present_info );
            
            try
            {
                if ( result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR ||
                    framebuffer_resized_ )
                {
                    framebuffer_resized_ = false;
    
                    const auto swapchain_create_info = vulkan::swapchain::create_info_type( )
                        .set_gpu( context_.gpu_ )
                        .set_device( context_.device_.get() )
                        .set_surface( context_.surface_.get() )
                        .set_render_pass( vk_context_.render_pass_.get() )
                        .set_width( window_width_ )
                        .set_height( window_height_ );
    
                    swapchain_.recreate( swapchain_create_info );
    
                    context_.device_->resetCommandPool( context_.graphics_command_pools_[0].get( ), { } );
    
                    record_draw_calls( );
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
    
    const vk::UniqueSemaphore renderer::create_semaphore( ) const noexcept
    {
        const auto create_info = vk::SemaphoreCreateInfo( );
        
        return context_.device_->createSemaphoreUnique( create_info );
    }
    
    const vk::UniqueFence renderer::create_fence( ) const noexcept
    {
        const auto create_info = vk::FenceCreateInfo( )
            .setFlags( vk::FenceCreateFlagBits::eSignaled );
        
        return context_.device_->createFenceUnique( create_info );
    }
    
    const vk::UniqueCommandPool renderer::create_command_pool( uint32_t queue_family ) const noexcept
    {
        const auto create_info = vk::CommandPoolCreateInfo( )
            .setQueueFamilyIndex( queue_family );
        
        return context_.device_->createCommandPoolUnique( create_info );
    }
    
    const std::vector<vk::CommandBuffer> renderer::create_command_buffers( uint32_t count ) const noexcept
    {
        const auto allocate_info = vk::CommandBufferAllocateInfo( )
            .setCommandPool( context_.graphics_command_pools_[0].get() )
            .setCommandBufferCount( count )
            .setLevel( vk::CommandBufferLevel::ePrimary );
        
        return context_.device_->allocateCommandBuffers( allocate_info );
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
        
        return context_.device_->createRenderPassUnique( create_info );
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
        return find_queue_family_indices( surface, physical_device ).has_rendering_support() &&
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
    
        const auto queue_properties = physical_device.getQueueFamilyProperties( );
        
        int i = 0;
        for( const auto& queue_property : queue_properties )
        {
            
            if( queue_property.queueCount > 0 )
            {
                if ( queue_property.queueFlags & vk::QueueFlagBits::eGraphics )
                {
                    indices.graphic_family_ = i;
                }
                
                if ( physical_device.getSurfaceSupportKHR( i, surface ) )
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
}