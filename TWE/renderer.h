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
#include "vk_shader_manager.h"
#include "vk_pipeline_manager.h"
#include "window/base_window.h"

namespace TWE
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
        std::uint32_t TWE_API create_shader( const std::string& filepath, const std::string& entry_point,
            const vk_shader::type& flags );

        void TWE_API draw_frame( );
        
        void TWE_API record_draw_calls( );
        
        void TWE_API on_window_close( const window_close_event& event );
        void TWE_API on_framebuffer_resize( const framebuffer_resize_event& event );
    
    private:
        void recreate_swapchain( );
        void cleanup_swapchain( );
        
        void set_up( );
        
        const vk::ResultValue<vk::UniqueInstance> create_instance( const std::string& app_name,
            uint32_t app_version ) const noexcept;
    
        const vk::ResultValue<vk::DebugReportCallbackEXT> create_debug_report( ) const noexcept;
    
        const vk::ResultValue<vk::UniqueSurfaceKHR> create_surface( const base_window* p_window ) const noexcept;
    
        const VkPhysicalDevice pick_physical_device( ) const noexcept;
    
        const vk::ResultValue<vk::UniqueDevice> create_device( ) const noexcept;
        
        const vk::ResultValue<vk::UniqueSemaphore> create_semaphore( ) const noexcept;
        
        const vk::ResultValue<vk::UniqueFence> create_fence( ) const noexcept;
        
        const vk::ResultValue<vk::UniqueCommandPool> create_command_pool( uint32_t queue_family ) const noexcept;
        
        const vk::ResultValue<std::vector<vk::CommandBuffer>> create_command_buffers( uint32_t count ) const noexcept;
        
        const vk::ResultValue<vk::UniqueSwapchainKHR> create_swapchain( const queue_family_indices_type& queue_family_indices_,
            const vk::PresentModeKHR & present_mode_, const vk::SurfaceCapabilitiesKHR& capabilities_,
            uint32_t image_count_ ) const noexcept;
        
        const vk::ResultValue<vk::UniqueImageView> create_image_view( const vk::Image& image ) const noexcept;
        
        const vk::ResultValue<vk::UniqueFramebuffer> create_framebuffer( const vk::ImageView& image_view ) const noexcept;
        
        const vk::ResultValue<vk::UniqueRenderPass> create_render_pass( ) const noexcept;
    
        const vk::ResultValue<vk::PipelineLayout> create_pipeline_layout( ) const noexcept;
    
        const vk::ResultValue<vk::Pipeline> create_graphics_pipeline(
            const vk::PipelineVertexInputStateCreateInfo vertex_input_info,
            std::uint32_t stage_count,
            const vk::PipelineShaderStageCreateInfo* p_stages ) const noexcept;
        
        
    

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
        
        size_t current_frame_ = 0;
        
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
    
            vk::PipelineLayout graphics_pipeline_layout_;
            vk::Pipeline graphics_pipeline_;
            
            std::vector<const char*> instance_extensions_;
            std::vector<const char*> device_extensions_;
            std::vector<const char*> validation_layers_;
        } vk_context_;
        
        vk_shader_manager shader_manager_;
        vk_pipeline_manager pipeline_manager_;
        
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