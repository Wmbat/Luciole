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

#include "mesh.hpp"
#include "renderer.hpp"
#include "vertex.hpp"
#include "../utilities/log.hpp"

#undef max

static std::vector<glm::vec3> positions = {
    { -.5f, -.5f, .0f },
    {  .5f, -.5f, .0f },
    {  .5f,  .5f, .0f },
    { -.5f,  .5f, .0f }
};

static std::vector<glm::vec4> colours = {
    { 1.f, .0f, .0f, 1.f },
    { .0f, 1.f, .0f, 1.f },
    { .0f, .0f, 1.f, 1.f },
    { 1.f, 1.f, 1.f, 1.f }
};

static std::vector<uint32_t> indices = {
    0, 1, 2, 2, 3, 0
};

static auto test_mesh = lcl::mesh( )
    .set_positions( positions )
    .set_colours( colours )
    .set_indices( indices );

namespace lcl
{    
    renderer::renderer( base_window* p_wnd, const std::string& app_name, uint32_t app_version )
        :
        window_width_( p_wnd->get_width() ),
        window_height_( p_wnd->get_height() ),
        clear_colour_( 0.0f, 0.0f, 0.0f, 1.0f )
    {
        //////////////////////////
        surface_ = vulkan::surface{ *p_wnd, instance_ };
        //device_ = vulkan::device{ instance_, surface_ };

        ///////////////////////////

/*
        p_wnd->set_event_callback( window_close_event_delg( *this, &renderer::on_window_close ) );
        p_wnd->set_event_callback( framebuffer_resize_event_delg( *this, &renderer::on_framebuffer_resize ) );
  */      
        /*
        const auto context_create_info = vulkan::context::create_info( )
            .set_p_window( p_wnd )
            .set_app_name( app_name )
            .set_app_version( app_version )
            .set_max_frames_in_flight( MAX_FRAMES_IN_FLIGHT );
        
        context_ = vulkan::context( context_create_info );
    
        const auto surface_format = choose_swapchain_surface_format( context_.gpu_.getSurfaceFormatsKHR( context_.surface_.get() ) );
        
        auto temp_render_pass = create_render_pass( surface_format );
        render_pass_ = std::move( temp_render_pass );

        const auto swapchain_create_info = vulkan::swapchain::create_info_type( )
            .set_gpu( context_.gpu_ )
            .set_device( context_.device_.get() )
            .set_surface( context_.surface_.get() )
            .set_render_pass( render_pass_.get() )
            .set_width( window_width_ )
            .set_height( window_height_ );
    
        swapchain_ = vulkan::swapchain( swapchain_create_info );
        
        image_available_semaphores_.resize( MAX_FRAMES_IN_FLIGHT );
        render_finished_semaphores_.resize( MAX_FRAMES_IN_FLIGHT );
        in_flight_fences_.resize( MAX_FRAMES_IN_FLIGHT );
    
        for( auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
        {
            auto image_available_semaphore = create_semaphore();
            image_available_semaphores_[i] = std::move( image_available_semaphore );
        
            auto render_finished_semaphore = create_semaphore();
            render_finished_semaphores_[i] = std::move( render_finished_semaphore );
        
            auto fence = create_fence();
            in_flight_fences_[i] = std::move( fence );
        }
        
        for( auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
        {
            auto render_command_buffers = create_command_buffers( context_.graphics_command_pools_[i].get( ), swapchain_.image_count_ ); 
            render_command_buffers_[i] = std::move( render_command_buffers );
        }
        
        auto transfer_command_buffers = create_command_buffers( context_.transfer_command_pool_.get(), 1 );
        transfer_command_buffers_ = std::move( transfer_command_buffers );

        auto mem_allocator_create_info = VmaAllocatorCreateInfo( );
        mem_allocator_create_info.physicalDevice = context_.gpu_;
        mem_allocator_create_info.device = context_.device_.get();
        
        VmaVulkanFunctions test;
        test.vkMapMemory = vkMapMemory;
        test.vkUnmapMemory = vkUnmapMemory;
        test.vkAllocateMemory = vkAllocateMemory;
        test.vkBindBufferMemory = vkBindBufferMemory;
        test.vkBindImageMemory = vkBindImageMemory;
        test.vkCmdCopyBuffer = vkCmdCopyBuffer;
        test.vkCreateBuffer = vkCreateBuffer;
        test.vkCreateImage = vkCreateImage;
        test.vkDestroyBuffer = vkDestroyBuffer;
        test.vkDestroyImage = vkDestroyImage;
        test.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
        test.vkFreeMemory = vkFreeMemory;
        test.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
        test.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
        test.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
        test.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
        
        mem_allocator_create_info.pVulkanFunctions = &test;
    
        memory_allocator_ = vulkan::memory_allocator( mem_allocator_create_info );
    
        const auto mesh_buffer_create_info = vulkan::mesh_buffer_create_info()
            .set_memory_allocator( memory_allocator_.get() )
            .set_transfer_queue( context_.transfer_queue_ )
            .set_transfer_command_buffer( transfer_command_buffers_[0].get() )
            .set_queue_family_index_count( context_.queue_family_count_ )
            .set_p_queue_family_indices( context_.queue_family_indices_.data() )
            .set_data( test_mesh );
        
        test_mesh_buffer_ = vulkan::mesh_buffer( mesh_buffer_create_info );
        */
    }
    renderer::renderer( renderer&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    renderer::~renderer( )
    {
        // context_.device_->waitIdle( );
    }
    
    renderer& renderer::operator=( renderer&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            window_width_ = rhs.window_width_;
            rhs.window_width_ = 0;
            
            window_height_ = rhs.window_height_;
            rhs.window_height_ = 0;
            
            /*
            clear_colour_ = std::move( rhs.clear_colour_ );
            
            in_flight_fences_ = std::move( rhs.in_flight_fences_ );
            image_available_semaphores_ = std::move( rhs.image_available_semaphores_ );
            render_finished_semaphores_ = std::move( rhs.render_finished_semaphores_ );
            
            for( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
            {
                render_command_buffers_[i] = std::move( rhs.render_command_buffers_[i] );
            }
            
            render_pass_ = std::move( rhs.render_pass_ );
            */
        }
    
        return *this;
    }
    
    /*
    std::uint32_t renderer::create_vertex_shader( const std::string_view filepath, const std::string_view entry_point )
    {
        return shader_manager_.insert<vulkan::shader_type::e_vertex>( vulkan::shader_create_info{ context_.device_.get(), filepath, entry_point } );
    }

    std::uint32_t renderer::create_fragment_shader( const std::string_view filepath, const std::string_view entry_point )
    {
        return shader_manager_.insert<vulkan::shader_type::e_fragment>( vulkan::shader_create_info{ context_.device_.get( ), filepath, entry_point } );
    }

    std::uint32_t renderer::create_graphics_pipeline( const std::string_view filepath, std::uint32_t vertex_shader_id, std::uint32_t fragment_shader_id )
    {
        std::vector<vk::Viewport> viewports = {
            vk::Viewport( )
                .setX( 0.0f )
                .setY( 0.0f )
                .setWidth( static_cast<float>( swapchain_.extent_.width ) )
                .setHeight( static_cast<float>( swapchain_.extent_.height ) )
                .setMinDepth( 0.0f )
                .setMaxDepth( 1.0f )
        };
    
        std::vector<vk::Rect2D> scissors = {
            vk::Rect2D( )
                .setOffset( { 0, 0 } )
                .setExtent( swapchain_.extent_ )
        };
            
        const auto create_info = vulkan::graphics_pipeline::create_info( )
            .set_device( context_.device_.get() )
            .set_render_pass( render_pass_.get() )
            .set_pipeline_definition( filepath )
            .set_shader_manager( &shader_manager_ )
            .set_shader_ids( vertex_shader_id, fragment_shader_id )
            .set_viewports( viewports )
            .set_scissors( scissors );
            
        return pipeline_manager_.insert<vulkan::pipeline_type::graphics>( create_info );
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
    
        for( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
        {
            for( size_t j = 0; j < render_command_buffers_[i].size(); ++j )
            {
                const auto begin_info = vk::CommandBufferBeginInfo( )
                    .setFlags( vk::CommandBufferUsageFlagBits::eSimultaneousUse );
        
                render_command_buffers_[i][j]->begin( begin_info );
    
                const auto clear_colour_value= vk::ClearColorValue( )
                    .setFloat32( std::array<float, 4>{ clear_colour_.r / 255.f, clear_colour_.g / 255.f, clear_colour_.b / 255.f, clear_colour_.a / 255.f } );
        
                const auto clear_colour = vk::ClearValue( )
                    .setColor( clear_colour_value );
        
                const auto render_pass_begin_info = vk::RenderPassBeginInfo( )
                    .setFramebuffer( swapchain_.framebuffers_[j].get() )
                    .setRenderPass( render_pass_.get() )
                    .setClearValueCount( 1 )
                    .setPClearValues( &clear_colour )
                    .setRenderArea( { { 0, 0 }, swapchain_.extent_ } );
        
                render_command_buffers_[i][j]->beginRenderPass( &render_pass_begin_info, vk::SubpassContents::eInline );
        
                const auto& pipeline = pipeline_manager_.find<vulkan::pipeline_type::graphics>( current_pipeline_ );

                pipeline.set_viewport( render_command_buffers_[i][j].get( ), 0, viewports );
                pipeline.set_scissors( render_command_buffers_[i][j].get( ), 0, scissors );

                pipeline.bind( render_command_buffers_[i][j].get() );
                
                std::array<vk::Buffer, 2> vertex_buffers = { test_mesh_buffer_.get( ), test_mesh_buffer_.get( ) };
                std::array<vk::DeviceSize, 2> offsets = { 
                    test_mesh_buffer_.get_offset<vulkan::mesh_buffer_attribute::position>( ),
                    test_mesh_buffer_.get_offset<vulkan::mesh_buffer_attribute::colour>( )
                };
                render_command_buffers_[i][j]->bindVertexBuffers( 0, vertex_buffers, offsets );

                const auto index_offset = test_mesh_buffer_.get_offset<vulkan::mesh_buffer_attribute::index>( );
                render_command_buffers_[i][j]->bindIndexBuffer( test_mesh_buffer_.get( ), index_offset, vk::IndexType::eUint32 );

                render_command_buffers_[i][j]->drawIndexed( static_cast<uint32_t>( indices.size() ), 1, 0, 0, 0 );
            
                render_command_buffers_[i][j]->endRenderPass( );
            
                render_command_buffers_[i][j]->end( );
            }
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
            .set_render_pass( render_pass_.get() )
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
            context_.device_->waitForFences( 1, &in_flight_fences_[current_frame_].get(),
                VK_TRUE, std::numeric_limits<uint64_t>::max() );

            auto [result, image_index] = context_.device_->acquireNextImageKHR(
                swapchain_.swapchain_.get(),
                std::numeric_limits<uint64_t>::max(),
                image_available_semaphores_[current_frame_].get(), {} );

            if ( result == vk::Result::eErrorOutOfDateKHR )
            {
                const auto swapchain_create_info = vulkan::swapchain::create_info_type( )
                    .set_gpu( context_.gpu_ )
                    .set_device( context_.device_.get() )
                    .set_surface( context_.surface_.get() )
                    .set_render_pass( render_pass_.get() )
                    .set_width( window_width_ )
                    .set_height( window_height_ );
    
                swapchain_.recreate( swapchain_create_info );
    
                for( auto& command_pool : context_.graphics_command_pools_ )
                {
                    context_.device_->resetCommandPool( command_pool.get(), { } );
                }
    
                record_draw_calls( );
            }
            else if ( result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR )
            {
                core_error( "Failed to acquire swapchain image" );
            }

            const vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits ::eColorAttachmentOutput };
            const auto submit_info = vk::SubmitInfo( )
                .setPWaitDstStageMask( wait_stages )
                .setWaitSemaphoreCount( 1 )
                .setPWaitSemaphores( &image_available_semaphores_[current_frame_].get() )
                .setSignalSemaphoreCount( 1 )
                .setPSignalSemaphores( &render_finished_semaphores_[current_frame_].get() )
                .setCommandBufferCount( 1 )
                .setPCommandBuffers( &render_command_buffers_[current_frame_][image_index].get( ) );

            context_.device_->resetFences( 1, &in_flight_fences_[current_frame_].get() );

            try
            {
                context_.graphics_queue_.submit( 1, &submit_info, in_flight_fences_[current_frame_].get()  );
            }
            catch ( const vulkan::error& e )
            {
                core_error ( e.what ( ) );
            }
            
            auto present_info = VkPresentInfoKHR{ };
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = reinterpret_cast<VkSemaphore*>( &render_finished_semaphores_[current_frame_].get() );
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
                        .set_render_pass( render_pass_.get() )
                        .set_width( window_width_ )
                        .set_height( window_height_ );
    
                    swapchain_.recreate( swapchain_create_info );
    
                    for( auto& command_pool : context_.graphics_command_pools_ )
                    {
                        context_.device_->resetCommandPool( command_pool.get(), { } );
                    }
    
                    record_draw_calls( );
                }
                else if ( result != vk::Result::eSuccess )
                {
                    throw vulkan::error{ result, "failed to present swapchain image." };
                }
            }
            catch ( const vulkan::error& e )
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

    const std::vector<vk::UniqueCommandBuffer> renderer::create_command_buffers( 
        const vk::CommandPool command_pool, 
        std::uint32_t count ) const
    {
        const auto allocate_info = vk::CommandBufferAllocateInfo( )
            .setCommandPool( command_pool )
            .setCommandBufferCount( count )
            .setLevel( vk::CommandBufferLevel::ePrimary );
    
        return context_.device_->allocateCommandBuffersUnique( allocate_info );
    }

    const vk::UniqueRenderPass renderer::create_render_pass( 
        const vk::SurfaceFormatKHR surface_format,
        const vk::PipelineBindPoint bind_point ) const noexcept
    {
        const auto colour_attachment = vk::AttachmentDescription( )
            .setFormat( surface_format.format )
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
    
        return std::move( context_.device_->createRenderPassUnique( create_info ) );
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
    */
}