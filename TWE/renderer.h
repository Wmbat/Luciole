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

#ifndef VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_EXCEPTIONS
#endif

#include <vulkan/vulkan.hpp>

#include "TWE_core.h"
#include "event/event_listeners.h"
#include "window/base_window.h"


namespace TWE
{
    class renderer : public framebuffer_resize_listener, public window_close_listener
    {
    public:
        struct graphics_pipeline_data;
    
    private:
        struct queue_family_indices_type;
        struct swapchain_support_details_type;

    public:
        TWE_API renderer( const base_window* p_window, const std::string& app_name, uint32_t app_version );
        renderer( const renderer& renderer ) noexcept = delete;
        TWE_API renderer( renderer&& renderer ) noexcept;
        TWE_API ~renderer( );

        renderer& operator=( const renderer& renderer ) noexcept = delete;
        TWE_API renderer& operator=( renderer&& renderer ) noexcept;
        
        void TWE_API setup_graphics_pipeline( const graphics_pipeline_data& data );

        void TWE_API draw_frame( const TWE::renderer::graphics_pipeline_data &data );
        
        void TWE_API record_draw_calls( );
        
        void TWE_API execute( const framebuffer_resize_event& event ) override;
        void TWE_API execute( const window_close_event& event ) override;
    
    private:
        void recreate_swapchain( const TWE::renderer::graphics_pipeline_data &data );
        void cleanup_swapchain( );
        
        void set_up( );
        
        const vk::ResultValue<vk::Instance> create_instance( const std::string& app_name,
            uint32_t app_version ) const noexcept;
    
        const vk::ResultValue<vk::DebugReportCallbackEXT> create_debug_report( ) const noexcept;
    
        const vk::ResultValue<vk::SurfaceKHR> create_surface( const base_window* p_window ) const noexcept;
    
        const VkPhysicalDevice pick_physical_device( ) const noexcept;
    
        const vk::ResultValue<vk::Device> create_device( ) const noexcept;
        
        const vk::ResultValue<vk::Semaphore> create_semaphore( ) const noexcept;
        
        const vk::ResultValue<vk::Fence> create_fence( ) const noexcept;
        
        const vk::ResultValue<vk::CommandPool> create_command_pool( uint32_t queue_family ) const noexcept;
        
        const vk::ResultValue<std::vector<vk::CommandBuffer>> create_command_buffers( uint32_t count ) const noexcept;
        
        const vk::ResultValue<vk::SwapchainKHR> create_swapchain( const queue_family_indices_type& queue_family_indices_,
            const vk::PresentModeKHR & present_mode_, const vk::SurfaceCapabilitiesKHR& capabilities_,
            uint32_t image_count_ ) const noexcept;
        
        const vk::ResultValue<vk::ImageView> create_image_view( const vk::Image& image ) const noexcept;
        
        const vk::ResultValue<vk::Framebuffer> create_framebuffer( const vk::ImageView& image_view ) const noexcept;
        
        const vk::ResultValue<vk::RenderPass> create_render_pass( ) const noexcept;
        
        const vk::ResultValue<vk::ShaderModule> create_shader_module( const std::string& filepath ) const noexcept;
    
        const vk::ResultValue<vk::PipelineLayout> create_pipeline_layout( ) const noexcept;
    
        const vk::ResultValue<vk::Pipeline> create_graphics_pipeline( std::uint32_t stage_count,
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
            vk::Instance instance_;
            vk::DebugReportCallbackEXT debug_report_;
            vk::SurfaceKHR surface_;
            vk::PhysicalDevice gpu_;
            vk::Device device_;
            vk::Queue graphics_queue_;
            vk::Queue present_queue_;
            
            std::vector<vk::Semaphore> image_available_semaphores_;
            std::vector<vk::Semaphore> render_finished_semaphores_;
            std::vector<vk::Fence> in_flight_fences_;
            
            vk::CommandPool command_pool_;
            std::vector<vk::CommandBuffer> command_buffers_;
            
            vk::SurfaceFormatKHR surface_format_;
            
            vk::SwapchainKHR swapchain_;
            vk::Extent2D swapchain_extent_;
            std::vector<vk::Image> swapchain_image_;
            std::vector<vk::ImageView> swapchain_image_views_;
            std::vector<vk::Framebuffer> swapchain_framebuffers_ = { };
            
            vk::RenderPass render_pass_;
    
            vk::PipelineLayout graphics_pipeline_layout_;
            vk::Pipeline graphics_pipeline_;
            
            std::vector<const char*> instance_extensions_;
            std::vector<const char*> device_extensions_;
            std::vector<const char*> validation_layers_;
        } vk_context_;
        
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
        struct graphics_pipeline_data
        {
            const std::string vertex_shader_filepath;
            const std::string fragment_shader_filepath;
        };
    };
}

#endif //VULKAN_PROJECT_RENDERER_H