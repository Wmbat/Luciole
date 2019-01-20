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

#ifndef VULKAN_PROJECT_RENDERER_H
#define VULKAN_PROJECT_RENDERER_H

#include <optional>

#include "vertex_buffer.hpp"
#include "../twe_core.hpp"
#include "../window/base_window.hpp"
#include "../vulkan/context.hpp"
#include "../vulkan/swapchain.hpp"
#include "../vulkan/memory_allocator.hpp"
#include "../vulkan/shader_manager.hpp"
#include "../vulkan/pipeline_manager.hpp"

namespace twe
{
    class renderer
    {
    private:
        struct queue_family_indices_type;
        struct swapchain_support_details_type;

    public:
        TWE_API renderer( base_window* p_window, const std::string& app_name, uint32_t app_version );
        TWE_API renderer( const renderer& renderer ) noexcept = delete;
        TWE_API renderer( renderer&& renderer ) noexcept;
        TWE_API ~renderer( );

        TWE_API renderer& operator=( const renderer& renderer ) noexcept = delete;
        TWE_API renderer& operator=( renderer&& renderer ) noexcept;
        
        template<vulkan::shader_type T>
        uint32_t create_shader( const std::string& filepath, const std::string& entry_point )
        {
            return shader_manager_.insert<T>( vulkan::shader_create_info{ context_.device_.get(), filepath, entry_point } );
        }
        
        template<vulkan::pipeline_type T>
        uint32_t create_pipeline( const std::string& pipeline_definition, uint32_t vert_id, uint32_t frag_id )
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
            
            const auto create_info = vulkan::pipeline_create_info( )
                .set_device( context_.device_.get() )
                .set_render_pass( render_pass_.get() )
                .set_pipeline_definition( pipeline_definition )
                .set_shader_manager( &shader_manager_ )
                .set_shader_ids( vert_id, frag_id )
                .set_viewports( viewports )
                .set_scissors( scissors );
            
            return pipeline_manager_.insert<T>( create_info );
        }
        
        TWE_API void set_pipeline( const uint32_t id );
        TWE_API void switch_pipeline( const uint32_t id );

        TWE_API void draw_frame( );
        
        TWE_API void record_draw_calls( );
        
        TWE_API void on_window_close( const window_close_event& event );
        TWE_API void on_framebuffer_resize( const framebuffer_resize_event& event );
        
        TWE_API void set_clear_colour( const glm::vec4& colour );
    
    private:
        const vk::UniqueSemaphore create_semaphore( ) const noexcept;
        
        const vk::UniqueFence create_fence( ) const noexcept;
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, vk::UniqueCommandBuffer>, std::vector<C>> create_handles( const vk::CommandPool command_pool, uint32_t count ) const noexcept
        {
            const auto allocate_info = vk::CommandBufferAllocateInfo( )
                .setCommandPool( command_pool )
                .setCommandBufferCount( count )
                .setLevel( vk::CommandBufferLevel::ePrimary );
    
            return context_.device_->allocateCommandBuffersUnique( allocate_info );
        }
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, vk::UniqueRenderPass>, C> create_handle(
            const vk::SurfaceFormatKHR surface_format,
            const vk::PipelineBindPoint bind_point = vk::PipelineBindPoint::eGraphics ) const noexcept
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
    
            return context_.device_->createRenderPassUnique( create_info );
        }
        
        const swapchain_support_details_type query_swapchain_support( const vk::SurfaceKHR& surface,
            const vk::PhysicalDevice &physical_device ) const noexcept;

        const vk::SurfaceFormatKHR choose_swapchain_surface_format(
                const std::vector<vk::SurfaceFormatKHR>& available_formats ) const noexcept;

    private:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        
        uint32_t window_width_;
        uint32_t window_height_;

        bool is_window_closed_ = false;
        bool framebuffer_resized_ = false;
    
        glm::vec4 clear_colour_;
        
        size_t current_frame_ = 0;
        
        uint32_t current_pipeline_;
    
        vulkan::context context_;
        vulkan::swapchain swapchain_;
    
        std::vector<vk::UniqueSemaphore> image_available_semaphores_;
        std::vector<vk::UniqueSemaphore> render_finished_semaphores_;
        std::vector<vk::UniqueFence> in_flight_fences_;
    
        std::vector<vk::UniqueCommandBuffer> render_command_buffers_[MAX_FRAMES_IN_FLIGHT];
    
        vk::UniqueRenderPass render_pass_;
        
        vulkan::memory_allocator memory_allocator_;
        
        vertex_buffer vertex_buffer_;
        
        vulkan::shader_manager shader_manager_;
        vulkan::pipeline_manager pipeline_manager_;
        
    private:
        
        struct queue_family_indices_type
        {
            std::optional<uint32_t> graphic_family_;
            std::optional<uint32_t> present_family_;
            
            bool has_rendering_support( ) const
            {
                return graphic_family_.has_value() && present_family_.has_value();
            }
        };
        struct swapchain_support_details_type
        {
            vk::SurfaceCapabilitiesKHR capabilities_;
            std::vector<vk::SurfaceFormatKHR> formats_;
            std::vector<vk::PresentModeKHR> present_modes_;
        };
    };
}

#endif //VULKAN_PROJECT_RENDERER_H