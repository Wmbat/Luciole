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

#include <luciole/graphics/renderer.hpp>
#include <luciole/graphics/vertex.hpp>
#include <luciole/ui/event.hpp>
#include <luciole/utilities/log.hpp>

#include <wmbats_bazaar/file_io.hpp>

const std::vector<vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

renderer::renderer( p_context_t p_context, ui::window& wnd )
    :
    p_context_( p_context.value_ )
{
    wnd.add_callback( framebuffer_resize_event_delg( *this, &renderer::on_framebuffer_resize ) );

    auto const capabilities = p_context_->get_surface_capabilities();
    auto const format = pick_swapchain_format();
    
    swapchain_image_format_ = format.format;
    swapchain_extent_ = pick_swapchain_extent( capabilities );
    
    std::uint32_t image_count = capabilities.minImageCount + 1;
    if ( capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount )
    {
        image_count = capabilities.maxImageCount;
    }
    
    swapchain_images_.reserve( image_count );
    swapchain_image_views_.reserve( image_count );
    swapchain_framebuffers_.reserve( image_count );
    
    /*
     *  Check for errors upon swapchain creation.
     */
    if ( auto res = create_swapchain( capabilities, format ); auto p_val = std::get_if<VkSwapchainKHR>( &res ) )
    {
        swapchain_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: On Swapchain creation -> Error code: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }
    
    /*
     *  Check for errors upon swapchain images retrieval.
     */
    if ( auto res = p_context_->get_swapchain_images( vk::swapchain_t( swapchain_ ), count32_t( image_count ) );
         auto p_val = std::get_if<std::vector<VkImage>>( &res ) )
    {
        swapchain_images_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: On Swapchain Images retrieval -> Error code: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }
    
    /*
     *  Check for erros upon Swapchain image views creation.
     */
    swapchain_image_views_.reserve( swapchain_images_.size( ) );
    for( std::size_t i = 0; i < swapchain_images_.size( ); ++i )
    {
        if ( auto res = create_image_view( vk::image_t( swapchain_images_[i] ) ); auto p_val = std::get_if<VkImageView>( &res ) )
        {
            swapchain_image_views_.emplace_back( *p_val );
        }
        else
        {
            core_error( "Vulkan Error: On Swapchain Image View {} creation -> Error code: {}.", i, vk::error::to_string( std::get<vk::error::type>( res ) ) );

            abort( );
        }
    }
    
    /*
     *  Check for any errors upon Render Pass creation.
     */
    if ( auto res = create_render_pass( ); auto p_val = std::get_if<VkRenderPass>( &res ) )
    {
        render_pass_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: On Render Pass creation -> Error code: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );
        
        abort( );
    }

    /*
     *  Check for any errors upon default graphics pipeline layout creation.
     */
    if ( auto res = create_default_pipeline_layout( ); auto p_val = std::get_if<VkPipelineLayout>( &res ) )
    {
        default_graphics_pipeline_layout_ = *p_val;
    } 
    else
    {
        core_error( "Vulkan Error: On Default Graphics pipeline layout creation -> Error code: {}.",
            vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }

    /*
     *  Check for any errors upon default graphics pipeline creation.
     */
    if ( auto res = create_default_pipeline( 
        vert_shader_filepath_const_ref_t( "../data/shaders/default_vert.spv" ), 
        frag_shader_filepath_const_ref_t( "../data/shaders/default_frag.spv" ) 
        ); auto p_val = std::get_if<VkPipeline>( &res ) )
    {
        default_graphics_pipeline_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: On Default Graphics Pipeline creation -> Error code: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }

    if ( auto res = p_context_->create_command_buffers( queue::flag_t( queue::flag::e_graphics ), count32_t( image_count ) ); 
         auto p_val = std::get_if<std::vector<VkCommandBuffer>>( &res ) )
    {
        render_command_buffers_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: On Render Command Buffers creation -> Error code: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }
    
    swapchain_framebuffers_.reserve( image_count );
    for( std::size_t i = 0; i < image_count; ++i )
    {
        if ( auto res = create_framebuffer( vk::image_view_t( swapchain_image_views_[i] ) ); auto p_val = std::get_if<VkFramebuffer>( &res ) )
        {
            swapchain_framebuffers_.emplace_back( std::move( *p_val ) );
        }
        else
        {
            core_error( "Vulkan Error: On Swapchain Framebuffer {} creation -> Error code:", i, vk::error::to_string( std::get<vk::error::type>( res ) ) );

            abort( );
        }
    }

    for( int i = 0; i < MAX_FRAMES_IN_FLIGHT_; ++i )
    {
        if ( auto res = create_semaphore( ); auto p_val = std::get_if<VkSemaphore>( &res ) )
        {
            image_available_semaphore_[i] = *p_val;
        }
        else
        {
            core_error( "Vulkan Error: On Image Available Semaphore {} creation -> Error code:", i, vk::error::to_string( std::get<vk::error::type>( res ) ) );

            abort( );
        }

        if ( auto res = create_semaphore( ); auto p_val = std::get_if<VkSemaphore>( &res ) )
        {
            render_finished_semaphore_[i] = *p_val;
        }
        else
        {
            core_error( "Vulkan Error: On Render Finished Semaphore {} creation -> Error code:", i, vk::error::to_string( std::get<vk::error::type>( res ) ) );

            abort( );
        }

        if ( auto res = create_fence( ); auto p_val = std::get_if<VkFence>( &res ) )
        {
            in_flight_fences[i] = *p_val;
        }
        else
        {
            core_error( "Vulkan Error: On In Flight Fence {} creation -> Error code:", i, vk::error::to_string( std::get<vk::error::type>( res ) ) );

            abort( );
        }
    }

    record_command_buffers( );
}
renderer::renderer( renderer&& rhs )
{
    *this = std::move( rhs );
}
renderer::~renderer( )
{
    for( int i = 0; i < MAX_FRAMES_IN_FLIGHT_; ++i )
    {
        if ( image_available_semaphore_[i] != VK_NULL_HANDLE )
        {
            p_context_->destroy_semaphore( vk::semaphore_t( image_available_semaphore_[i] ) );
            image_available_semaphore_[i] = VK_NULL_HANDLE;
        }

        if ( render_finished_semaphore_[i] != VK_NULL_HANDLE )
        {
            p_context_->destroy_semaphore( vk::semaphore_t( render_finished_semaphore_[i] ) );
            render_finished_semaphore_[i] = VK_NULL_HANDLE;
        }

        if ( in_flight_fences[i] != VK_NULL_HANDLE )
        {
            p_context_->destroy_fence( vk::fence_t( in_flight_fences[i] ) );
            in_flight_fences[i] = VK_NULL_HANDLE;
        }
    }

    for( auto& framebuffer : swapchain_framebuffers_ )
    {
        if ( framebuffer != VK_NULL_HANDLE )
        {
            p_context_->destroy_framebuffer( vk::framebuffer_t( framebuffer ) );
            framebuffer = VK_NULL_HANDLE;
        }
    }

    if ( default_graphics_pipeline_ != VK_NULL_HANDLE )
    {
        p_context_->destroy_pipeline( vk::pipeline_t( default_graphics_pipeline_ ) );
        default_graphics_pipeline_ = VK_NULL_HANDLE;
    }

    if ( default_graphics_pipeline_layout_ != VK_NULL_HANDLE )
    {
        p_context_->destroy_pipeline_layout( vk::pipeline_layout_t( default_graphics_pipeline_layout_ ) );
        default_graphics_pipeline_layout_ = VK_NULL_HANDLE;
    }

    if ( render_pass_ != VK_NULL_HANDLE )
    {
        p_context_->destroy_render_pass( vk::render_pass_t( render_pass_ ) );
        render_pass_ = VK_NULL_HANDLE;
    }
    
    for( auto& image_view : swapchain_image_views_ )
    {
        if ( image_view != VK_NULL_HANDLE )
        {
            p_context_->destroy_image_view( vk::image_view_t( image_view ) );
            image_view = VK_NULL_HANDLE;
        }
    }
    
    if ( swapchain_ != VK_NULL_HANDLE )
    {
        p_context_->destroy_swapchain( vk::swapchain_t( swapchain_ ) );
        swapchain_ = VK_NULL_HANDLE;
    }
}

renderer& renderer::operator=( renderer&& rhs )
{
    if ( this != &rhs )
    {
        std::swap( swapchain_, rhs.swapchain_ );
        std::swap( swapchain_images_, rhs.swapchain_images_ );
        std::swap( swapchain_extent_, rhs.swapchain_extent_ );
        std::swap( swapchain_image_format_, rhs.swapchain_image_format_ );
        std::swap( swapchain_image_views_, rhs.swapchain_image_views_ );
        std::swap( render_pass_, rhs.render_pass_ );
        std::swap( default_graphics_pipeline_, rhs.default_graphics_pipeline_ );
        std::swap( default_graphics_pipeline_layout_, rhs.default_graphics_pipeline_layout_ );
        std::swap( swapchain_framebuffers_, rhs.swapchain_framebuffers_ );
        std::swap( render_command_buffers_, rhs.render_command_buffers_ );
        
        
        for( int i = 0; i < MAX_FRAMES_IN_FLIGHT_; ++i )
        {
            image_available_semaphore_[i] = rhs.image_available_semaphore_[i];
            rhs.image_available_semaphore_[i] = VK_NULL_HANDLE;

            render_finished_semaphore_[i] = rhs.render_finished_semaphore_[i];
            rhs.render_finished_semaphore_[i] = VK_NULL_HANDLE;

            in_flight_fences[i] = rhs.in_flight_fences[i];
            rhs.in_flight_fences[i] = VK_NULL_HANDLE;
        }

        p_context_ = rhs.p_context_;
        rhs.p_context_ = nullptr;
    }
    
    return *this;
}

void renderer::draw_frame( )
{
    p_context_->reset_fence( vk::fence_t( in_flight_fences[current_frame] ) );

    std::uint32_t image_index = 0;
    auto result = vkAcquireNextImageKHR( 
        p_context_->get( ), 
        swapchain_, 
        std::numeric_limits<std::uint64_t>::max( ), 
        image_available_semaphore_[current_frame], 
        VK_NULL_HANDLE, 
        &image_index 
    );

    if ( result == VK_ERROR_OUT_OF_DATE_KHR )
    {
        recreate_swapchain( );
        return;
    }

    VkSemaphore wait_semaphores[] = { image_available_semaphore_[current_frame] };
    VkSemaphore signal_semaphores[] = { render_finished_semaphore_[current_frame] };
    VkSwapchainKHR swapchains[] = { swapchain_ };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo const submit_info 
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = sizeof( wait_semaphores ) / sizeof( VkSemaphore ),
        .pWaitSemaphores = wait_semaphores,
        .pWaitDstStageMask = wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &render_command_buffers_[image_index],
        .signalSemaphoreCount = sizeof( signal_semaphores ) / sizeof( VkSemaphore ),
        .pSignalSemaphores = signal_semaphores
    };

    auto submit_result = p_context_->submit_queue( 
        queue::flag_t( queue::flag::e_graphics ), 
        vk::submit_info_t( submit_info ), 
        vk::fence_t( in_flight_fences[current_frame] )
    );

    if ( submit_result != vk::error::type::e_none )
    {
        abort();
    }

    p_context_->wait_for_fence( vk::fence_t( in_flight_fences[current_frame] ) );

    VkPresentInfoKHR const present_info 
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = sizeof( signal_semaphores ) / sizeof( VkSemaphore ),
        .pWaitSemaphores = signal_semaphores,
        .swapchainCount = sizeof( swapchains ) / sizeof( VkSwapchainKHR ),
        .pSwapchains = swapchains,
        .pImageIndices = &image_index,
        .pResults = nullptr
    };

    auto present_res = p_context_->present_queue( queue::flag_t( queue::flag::e_graphics ), vk::present_info_t( present_info ) );
    if ( present_res == vk::error::type::e_out_of_date || present_res == vk::error::type::e_suboptimal || is_framebuffer_resized_ )
    {
        is_framebuffer_resized_ = false;
        recreate_swapchain( );
    }
    else if ( present_res == vk::error::type::e_none )
    {
        current_frame = ( current_frame + 1 ) % MAX_FRAMES_IN_FLIGHT_;   
    }
    else
    {
        core_error( "Vulkan Error: On Present Queue -> Error code: {}.", vk::error::to_string( present_res ) );

        abort( );
    }
}

void renderer::on_framebuffer_resize( framebuffer_resize_event const& event )
{
    window_width_ = event.size_.x;
    window_height_ = event.size_.y;
    is_framebuffer_resized_ = true;
}

void renderer::recreate_swapchain( )
{
    vkDeviceWaitIdle( p_context_->get( ) );

    cleanup_swapchain( );

    auto const capabilities = p_context_->get_surface_capabilities();
    auto const format = pick_swapchain_format();

    swapchain_image_format_ = format.format;
    swapchain_extent_ = pick_swapchain_extent( capabilities );
    
    std::uint32_t image_count = capabilities.minImageCount + 1;
    if ( capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount )
    {
        image_count = capabilities.maxImageCount;
    }

    if ( auto res = create_swapchain( capabilities, format ); auto p_val = std::get_if<VkSwapchainKHR>( &res ) )
    {
        swapchain_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: On Swapchain creation -> Error code: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }

    swapchain_images_.reserve( image_count );
    if ( auto res = p_context_->get_swapchain_images( vk::swapchain_t( swapchain_ ), count32_t( image_count ) );
         auto p_val = std::get_if<std::vector<VkImage>>( &res ) )
    {
        swapchain_images_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: On Swapchain Images retrieval -> Error code: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }
    
    swapchain_image_views_.reserve( swapchain_images_.size( ) );
    for( std::size_t i = 0; i < swapchain_images_.size( ); ++i )
    {
        if ( auto res = create_image_view( vk::image_t( swapchain_images_[i] ) ); auto p_val = std::get_if<VkImageView>( &res ) )
        {
            swapchain_image_views_.emplace_back( *p_val );
        }
        else
        {
            core_error( "Vulkan Error: On Swapchain Image View {} creation -> Error code: {}.", i, vk::error::to_string( std::get<vk::error::type>( res ) ) );

            abort( );
        }
    }

        if ( auto res = create_render_pass( ); auto p_val = std::get_if<VkRenderPass>( &res ) )
    {
        render_pass_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: On Render Pass creation -> Error code: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );
        
        abort( );
    }

    if ( auto res = create_default_pipeline_layout( ); auto p_val = std::get_if<VkPipelineLayout>( &res ) )
    {
        default_graphics_pipeline_layout_ = *p_val;
    } 
    else
    {
        core_error( "Vulkan Error: On Default Graphics pipeline layout creation -> Error code: {}.",
            vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }

    if ( auto res = create_default_pipeline( 
        vert_shader_filepath_const_ref_t( "../data/shaders/default_vert.spv" ), 
        frag_shader_filepath_const_ref_t( "../data/shaders/default_frag.spv" ) );
        auto p_val = std::get_if<VkPipeline>( &res ) )
    {
        default_graphics_pipeline_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: On Default Graphics Pipeline creation -> Error code: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }

    if ( auto res = p_context_->create_command_buffers( queue::flag_t( queue::flag::e_graphics ), count32_t( image_count ) ); 
         auto p_val = std::get_if<std::vector<VkCommandBuffer>>( &res ) )
    {
        render_command_buffers_ = *p_val;
    }
    else
    {
        core_error( "Vulkan Error: On Render Command Buffers creation -> Error code: {}.", vk::error::to_string( std::get<vk::error::type>( res ) ) );

        abort( );
    }
    
    swapchain_framebuffers_.reserve( image_count );
    for( std::size_t i = 0; i < image_count; ++i )
    {
        if ( auto res = create_framebuffer( vk::image_view_t( swapchain_image_views_[i] ) ); auto p_val = std::get_if<VkFramebuffer>( &res ) )
        {
            swapchain_framebuffers_.emplace_back( std::move( *p_val ) );
        }
        else
        {
            core_error( "Vulkan Error: On Swapchain Framebuffer {} creation -> Error code:", i, vk::error::to_string( std::get<vk::error::type>( res ) ) );

            abort( );
        }
    }

    record_command_buffers( );
}
void renderer::cleanup_swapchain( )
{
    for( auto& framebuffer : swapchain_framebuffers_ )
    {
        if ( framebuffer != VK_NULL_HANDLE )
        {
            p_context_->destroy_framebuffer( vk::framebuffer_t( framebuffer ) );
            framebuffer = VK_NULL_HANDLE;
        }
    }
    swapchain_framebuffers_.clear( );

    if ( default_graphics_pipeline_ != VK_NULL_HANDLE )
    {
        p_context_->destroy_pipeline( vk::pipeline_t( default_graphics_pipeline_ ) );
        default_graphics_pipeline_ = VK_NULL_HANDLE;
    }

    if ( default_graphics_pipeline_layout_ != VK_NULL_HANDLE )
    {
        p_context_->destroy_pipeline_layout( vk::pipeline_layout_t( default_graphics_pipeline_layout_ ) );
        default_graphics_pipeline_layout_ = VK_NULL_HANDLE;
    }

    if ( render_pass_ != VK_NULL_HANDLE )
    {
        p_context_->destroy_render_pass( vk::render_pass_t( render_pass_ ) );
        render_pass_ = VK_NULL_HANDLE;
    }
    
    for( auto& image_view : swapchain_image_views_ )
    {
        if ( image_view != VK_NULL_HANDLE )
        {
            p_context_->destroy_image_view( vk::image_view_t( image_view ) );
            image_view = VK_NULL_HANDLE;
        }
    }
    swapchain_image_views_.clear( );
    
    if ( swapchain_ != VK_NULL_HANDLE )
    {
        p_context_->destroy_swapchain( vk::swapchain_t( swapchain_ ) );
        swapchain_ = VK_NULL_HANDLE;
    }
    swapchain_images_.clear( );
}

void renderer::record_command_buffers( )
{
    for( size_t i = 0; i < render_command_buffers_.size( ); ++i )
    {
        VkCommandBufferBeginInfo const buffer_begin_info 
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };

        if ( vkBeginCommandBuffer( render_command_buffers_[i], &buffer_begin_info ) != VK_SUCCESS )
        {
            core_error( "Failed to begin recording command buffer!" );
        }

        VkClearValue const clear_colour = { 0.0f, 0.0f, 0.0f, 1.0f };

        VkRenderPassBeginInfo const pass_begin_info 
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = render_pass_,
            .framebuffer = swapchain_framebuffers_[i],
            .renderArea = VkRect2D 
            {
                .offset = {0, 0},
                .extent = swapchain_extent_
            },
            .clearValueCount = 1,
            .pClearValues = &clear_colour
        };

        vkCmdBeginRenderPass( render_command_buffers_[i], &pass_begin_info, VK_SUBPASS_CONTENTS_INLINE );

        vkCmdBindPipeline( render_command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, default_graphics_pipeline_ );

        vkCmdDraw( render_command_buffers_[i], 3, 1, 0, 0 );

        vkCmdEndRenderPass( render_command_buffers_[i] );

        if ( vkEndCommandBuffer( render_command_buffers_[i] ) != VK_SUCCESS )
        {
            core_error( "Failed to record command buffer!" );
        }
    }
}

vk::error_variant<VkSwapchainKHR> renderer::create_swapchain( 
    VkSurfaceCapabilitiesKHR const& capabilities, 
    VkSurfaceFormatKHR const& format ) const 
{
    auto const present_mode = pick_swapchain_present_mode( );
    
    auto create_info = p_context_->swapchain_create_info( );
    create_info.minImageCount = static_cast<std::uint32_t>( swapchain_images_.capacity( ) );
    create_info.imageFormat = format.format;
    create_info.imageColorSpace = format.colorSpace;
    create_info.imageExtent = swapchain_extent_;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = nullptr;
    
    return p_context_->create_swapchain( vk::swapchain_create_info_t( create_info ) );
}

std::variant<VkImageView, vk::error::type> renderer::create_image_view( vk::image_t image ) const
{
    VkImageViewCreateInfo create_info
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = { },
        .image = image.value_,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = swapchain_image_format_,
        .components = VkComponentMapping
        {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = VkImageSubresourceRange
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    return p_context_->create_image_view( vk::image_view_create_info_t( create_info ) );
}

std::variant<VkRenderPass, vk::error::type> renderer::create_render_pass( ) const
{
    VkAttachmentDescription const colour_attachment
    {
        .flags = 0,
        .format = swapchain_image_format_,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };
    
    VkAttachmentReference const colour_attachment_reference
    {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    
    VkSubpassDescription const subpass_description
    {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colour_attachment_reference,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr
    };

    VkSubpassDependency const dependency 
    {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0
    };
    
    VkRenderPassCreateInfo const create_info
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .attachmentCount = 1,
        .pAttachments = &colour_attachment,
        .subpassCount = 1,
        .pSubpasses = &subpass_description,
        .dependencyCount = 1,
        .pDependencies = &dependency
    };
    
    return p_context_->create_render_pass( vk::render_pass_create_info_t( create_info ) );
}

VkShaderModule renderer::create_shader_module( shader_filepath_const_ref_t filepath ) const
{
    auto const spirv_code = bzr::read_from_binary_file( filepath.value_ );

    VkShaderModuleCreateInfo const create_info 
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = { },
        .codeSize = static_cast<std::uint32_t>( spirv_code.size( ) ),
        .pCode = reinterpret_cast<const std::uint32_t*>( spirv_code.data( ) )
    };

    return p_context_->create_shader_module( vk::shader_module_create_info_t( create_info ) );
}

std::variant<VkPipelineLayout, vk::error::type> renderer::create_default_pipeline_layout( ) const
{
    VkPipelineLayoutCreateInfo const create_info 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    return p_context_->create_pipeline_layout( vk::pipeline_layout_create_info_t( create_info ) );
}

std::variant<VkPipeline, vk::error::type> renderer::create_default_pipeline( 
    vert_shader_filepath_const_ref_t vert_filepath, 
    frag_shader_filepath_const_ref_t frag_filepath ) const 
{
    auto const vert_shader = create_shader_module( shader_filepath_const_ref_t( vert_filepath.value_ ) );
    auto const frag_shader = create_shader_module( shader_filepath_const_ref_t( frag_filepath.value_ ) );

    VkPipelineShaderStageCreateInfo vert_shader_stage_create_info 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vert_shader,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    VkPipelineShaderStageCreateInfo frag_shader_stage_create_info
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = frag_shader,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    VkPipelineShaderStageCreateInfo shader_stage_create_infos[] = 
    { 
        vert_shader_stage_create_info,
        frag_shader_stage_create_info
    };

    auto const binding_description = vertex::get_binding_description();
    auto const attribute_descriptions = vertex::get_attribute_descriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &binding_description,
        .vertexAttributeDescriptionCount = static_cast<std::uint32_t>(attribute_descriptions.size()),
        .pVertexAttributeDescriptions = attribute_descriptions.data()
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = { },
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    VkViewport viewport 
    {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>( swapchain_extent_.width ),
        .height = static_cast<float>( swapchain_extent_.height ),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor
    {
        .offset = { 0, 0 },
        .extent = swapchain_extent_
    };

    VkPipelineViewportStateCreateInfo viewport_state_create_info 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };

    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo multisample_state_create_info
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };

    VkPipelineColorBlendAttachmentState colour_blend_attachment_state
    {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo colour_blend_state_create_info
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colour_blend_attachment_state,
        .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
    };

    VkDynamicState dynamic_states[] = 
    {
        
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_create_info
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = sizeof ( dynamic_states ) / sizeof ( VkDynamicState ),
        .pDynamicStates = dynamic_states
    };

    VkGraphicsPipelineCreateInfo create_info 
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stageCount = 2,
        .pStages = shader_stage_create_infos,
        .pVertexInputState = &vertex_input_state_create_info,
        .pInputAssemblyState = &input_assembly_state_create_info,
        .pTessellationState = nullptr,
        .pViewportState = &viewport_state_create_info,
        .pRasterizationState = &rasterization_state_create_info,
        .pMultisampleState = &multisample_state_create_info,
        .pDepthStencilState = nullptr,
        .pColorBlendState = &colour_blend_state_create_info,
        .pDynamicState = &dynamic_state_create_info,
        .layout = default_graphics_pipeline_layout_,
        .renderPass = render_pass_,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0
    };

    auto const handle = p_context_->create_pipeline( vk::graphics_pipeline_create_info_t( create_info ) );

    p_context_->destroy_shader_module( vk::shader_module_t( frag_shader ) );
    p_context_->destroy_shader_module( vk::shader_module_t( vert_shader ) );

    return handle;
}

std::variant<VkSemaphore, vk::error::type> renderer::create_semaphore( ) const
{
    VkSemaphoreCreateInfo const create_info 
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };
    
    return p_context_->create_semaphore( vk::semaphore_create_info_t( create_info ) );
}

std::variant<VkFence, vk::error::type> renderer::create_fence( ) const noexcept
{
    VkFenceCreateInfo const create_info 
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    return p_context_->create_fence( vk::fence_create_info_t( create_info ) );
}

std::variant<VkFramebuffer, vk::error::type> renderer::create_framebuffer( vk::image_view_t image_view ) const noexcept
{
    VkImageView attachments[] = {
        image_view.value_
    };

    VkFramebufferCreateInfo const create_info
    {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderPass = render_pass_,
        .attachmentCount = sizeof( attachments ) / sizeof( VkImageView ),
        .pAttachments = attachments,
        .width = swapchain_extent_.width,
        .height = swapchain_extent_.height,
        .layers = 1
    };

    return p_context_->create_framebuffer( vk::framebuffer_create_info_t( create_info ) );
}

VkSurfaceFormatKHR renderer::pick_swapchain_format( ) const
{
    auto const formats = p_context_->get_surface_format( );
    
    for ( auto const& format : formats )
    {
        if ( format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
        {
            return format;
        }
    }
    
    return formats[0];
}

VkPresentModeKHR renderer::pick_swapchain_present_mode( ) const
{
    auto const present_modes = p_context_->get_present_modes( );
    
    for( auto const& present_mode : present_modes )
    {
        if ( present_mode == VK_PRESENT_MODE_MAILBOX_KHR )
        {
            return present_mode;
        }
    }
    
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D renderer::pick_swapchain_extent( VkSurfaceCapabilitiesKHR const& capabilities ) const
{
    if ( capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max( ) )
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actual_extent = { window_width_, window_height_ };
        
        actual_extent.width = std::clamp( actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
        actual_extent.height = std::clamp( actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );
        
        return actual_extent;
    }
}

