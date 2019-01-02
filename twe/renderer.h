/*!
 *  Copyright (C) 2018 Wmbat
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

#include "TWE_core.h"
#include "vk_utils.h"
#include "graphics/shader_manager.h"
#include "graphics/pipeline_manager.h"
#include "window/base_window.h"

namespace twe
{
    class renderer
    {
    public:
        struct shader_data_type;
    
    private:
        struct queue_family_indices_type;
        struct swapchain_support_details_type;

    public:
        TWE_API renderer( base_window* p_window, const std::string& app_name, uint32_t app_version );
        renderer( const renderer& renderer ) noexcept = delete;
        TWE_API renderer( renderer&& renderer ) noexcept;
        TWE_API ~renderer( );

        renderer& operator=( const renderer& renderer ) noexcept = delete;
        TWE_API renderer& operator=( renderer&& renderer ) noexcept;
        
        void TWE_API setup_graphics_pipeline( const shader_data_type& data );
        
        /*!
         * @brief Add a TWE::shader to the shader manager, if said TWE::shader is already present,
         * it will simply return the TWE::shader::id of said TWE:shader.
         * @throw Will throw a TWE::basic_error if the requested TWE::shader is not present.
         * @tparam T The TWE::shader::type of the TWE::shader
         * @param filepath The filepath to the TWE::shader.
         * @param entry_point The main function of the TWE::shader.
         * @return the id of the TWE::shader created or already present.
         */
        template<shader::type T>
        shader::id create_shader( const std::string& filepath, const std::string& entry_point )
        {
            return shader_manager_.insert( { vk_context_.device_.get(), T, filepath, entry_point } );
        }
        
        template<pipeline::type T, size_t number>
        std::vector<pipeline::id> create_pipelines(
            const shader::id vert_shader_id, const shader::id frag_shader_id,
            std::vector<std::string>& pipeline_definitions )
        {
            auto create_info = pipeline_manager::pipeline_create_info( )
                .set_shaders( vert_shader_id, frag_shader_id )
                .set_shader_manager( &shader_manager_ )
                .set_pipeline_type( T )
                .set_pipeline_definitions( pipeline_definitions )
                .set_extent( vk_context_.swapchain_.extent_ )
                .set_device( vk_context_.device_.get() )
                .set_render_pass( vk_context_.render_pass_.get() );
    
            return pipeline_manager_.insert<number>( create_info );
        }
        
        void TWE_API switch_pipeline( const pipeline::id id );

        void TWE_API draw_frame( );
        
        void TWE_API record_draw_calls( );
        
        void TWE_API on_window_close( const window_close_event& event );
        void TWE_API on_framebuffer_resize( const framebuffer_resize_event& event );
        
        void TWE_API set_clear_colour( const glm::vec4& colour );
    
    private:
        void recreate_swapchain( );
        void cleanup_swapchain( );
        
        void set_up( );
        
        const vk::UniqueInstance create_instance( const std::string& app_name, uint32_t app_version ) const noexcept;
    
        const vk::ResultValue<vk::DebugReportCallbackEXT> create_debug_report( ) const noexcept;
    
        const vk::PhysicalDevice pick_physical_device( ) const noexcept;
    
        const vk::UniqueDevice create_device( ) const noexcept;
        
        const vk::UniqueSemaphore create_semaphore( ) const noexcept;
        
        const vk::UniqueFence create_fence( ) const noexcept;
        
        const vk::UniqueCommandPool create_command_pool( uint32_t queue_family ) const noexcept;
        
        const std::vector<vk::CommandBuffer> create_command_buffers( uint32_t count ) const noexcept;
        
        const vk::UniqueSwapchainKHR create_swapchain( const queue_family_indices_type& queue_family_indices_,
            const vk::PresentModeKHR & present_mode_, const vk::SurfaceCapabilitiesKHR& capabilities_, uint32_t image_count_ ) const noexcept;
        
        const vk::UniqueImageView create_image_view( const vk::Image& image ) const noexcept;
        
        const vk::UniqueFramebuffer create_framebuffer( const vk::ImageView& image_view ) const noexcept;
        
        const vk::UniqueRenderPass create_render_pass( vk::PipelineBindPoint bind_point = vk::PipelineBindPoint::eGraphics ) const noexcept;
    

        bool check_instance_extension_support( const std::vector<const char*>& instance_extensions ) const noexcept;
    
        bool check_debug_layer_support( const std::vector<const char*>& debug_layers ) const noexcept;
    
        bool is_physical_device_suitable( const vk::SurfaceKHR& surface, const vk::PhysicalDevice& physical_device,
            const std::vector<const char*>& device_extensions ) const noexcept;
    
        bool check_physical_device_extension_support( const vk::PhysicalDevice& physical_device,
            const std::vector<const char*>& device_extensions ) const noexcept;
    
        bool is_swapchain_adequate( const vk::SurfaceKHR& surface,
            const vk::PhysicalDevice &physical_device ) const noexcept;
    
        const queue_family_indices_type find_queue_family_indices( const vk::SurfaceKHR& surface,
            const vk::PhysicalDevice &physical_device ) const noexcept;

        const swapchain_support_details_type query_swapchain_support( const vk::SurfaceKHR& surface,
            const vk::PhysicalDevice &physical_device ) const noexcept;

        const vk::SurfaceFormatKHR choose_swapchain_surface_format(
                const std::vector<vk::SurfaceFormatKHR>& available_formats ) const noexcept;

        const vk::PresentModeKHR choose_swapchain_present_mode(
                const std::vector<vk::PresentModeKHR>& available_present_modes ) const noexcept;

        const vk::Extent2D choose_swapchain_extent( const vk::SurfaceCapabilitiesKHR& capabilities ) const noexcept;

    private:
        uint32_t window_width_;
        uint32_t window_height_;

        bool is_window_closed_ = false;
        bool framebuffer_resized_ = false;
        
        vk::ClearValue clear_value_;
        
        size_t current_frame_ = 0;
        
        pipeline::id current_pipeline_;
        
        struct vk_context_t
        {
            vk::UniqueInstance instance_;
            vk::DebugReportCallbackEXT debug_report_;
            vk::UniqueSurfaceKHR surface_;
            vk::PhysicalDevice gpu_;
            vk::UniqueDevice device_;
            vk::Queue graphics_queue_;
            vk::Queue present_queue_;
            
            std::vector<vk::UniqueSemaphore> image_available_semaphores_;
            std::vector<vk::UniqueSemaphore> render_finished_semaphores_;
            std::vector<vk::UniqueFence> in_flight_fences_;
            
            vk::UniqueCommandPool command_pool_;
            std::vector<vk::CommandBuffer> command_buffers_;
            
            vk::SurfaceFormatKHR surface_format_;
            
            struct swapchain
            {
                vk::UniqueSwapchainKHR swapchain_;
                
                vk::Extent2D extent_;
                
                std::vector<vk::Image> image_;
                std::vector<vk::UniqueImageView> image_views_;
                std::vector<vk::UniqueFramebuffer> framebuffers_;
            } swapchain_;
            
            vk::UniqueRenderPass render_pass_;
            
            std::vector<const char*> instance_extensions_;
            std::vector<const char*> device_extensions_;
            std::vector<const char*> validation_layers_;
        } vk_context_;
    
        shader_manager shader_manager_;
        pipeline_manager pipeline_manager_;
        
    private:
        struct queue_family_indices_type
        {
            std::optional<uint32_t> graphic_family_;
            std::optional<uint32_t> compute_family_;
            std::optional<uint32_t> present_family_;
            
            bool is_complete( ) const
            {
                return graphic_family_.has_value() && compute_family_.has_value() && present_family_.has_value();
            }
        };
        struct swapchain_support_details_type
        {
            vk::SurfaceCapabilitiesKHR capabilities_;
            std::vector<vk::SurfaceFormatKHR> formats_;
            std::vector<vk::PresentModeKHR> present_modes_;
        };
    
    public:
        struct shader_data_type
        {
            const uint32_t vert_shader_id_;
            const uint32_t frag_shader_id_;
            const std::uint32_t vertex_position_binding_;
            const std::uint32_t vertex_position_location_;
            const std::uint32_t vertex_colour_binding_;
            const std::uint32_t vertex_colour_location_;
        };
    };
}

#endif //VULKAN_PROJECT_RENDERER_H