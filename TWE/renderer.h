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

#include <vulkan/vulkan.hpp>

#include "window.h"

namespace TWE
{
    class renderer
    {
    public:
        struct graphics_pipeline_data;
    
    private:
        struct swapchain_creation;
        struct queue_family_indices_type;
        struct swapchain_support_details_type;

    public:
        renderer( const window& window, const std::string& app_name, uint32_t app_version );
        renderer( const renderer& renderer ) noexcept = delete;
        renderer( renderer&& renderer ) noexcept;
        ~renderer( );

        renderer& operator=( const renderer& renderer ) noexcept = delete;
        renderer& operator=( renderer&& renderer ) noexcept;
        
        void setup_graphics_pipeline( const graphics_pipeline_data& data );

        void draw_frame( );
        
        void record_draw_calls( );
    
    private:
        void set_up( );
        
        const vk_return_type<VkInstance> create_instance( const std::string& app_name,
            uint32_t app_version ) const noexcept;
    
        const vk_return_type<VkDebugReportCallbackEXT> create_debug_report( ) const noexcept;
    
        const vk_return_type<VkSurfaceKHR> create_surface( const window& wnd ) const noexcept;
    
        const VkPhysicalDevice pick_physical_device( ) const noexcept;
    
        const vk_return_type<VkDevice> create_device( ) const noexcept;
        
        const vk_return_type<VkSemaphore> create_semaphore( ) const noexcept;
        
        const vk_return_type<VkFence> create_fence( ) const noexcept;
        
        const vk_return_type<VkCommandPool> create_command_pool( uint32_t queue_family ) const noexcept;
        
        const vk_return_type<std::vector<VkCommandBuffer>> create_command_buffers( uint32_t count ) const noexcept;
        
        const vk_return_type<VkSwapchainKHR> create_swapchain( const queue_family_indices_type& queue_family_indices_,
            const VkPresentModeKHR& present_mode_, const VkSurfaceCapabilitiesKHR& capabilities_,
            uint32_t image_count_ ) const noexcept;
        
        const vk_return_type<VkImageView> create_image_view( const VkImage& image ) const noexcept;
        
        const vk_return_type<VkFramebuffer> create_framebuffer( const VkImageView& image_view ) const noexcept;
        
        const vk_return_type<VkRenderPass> create_render_pass( ) const noexcept;
        
        const vk_return_type<VkShaderModule> create_shader_module( const std::string& filepath ) const noexcept;
    
        const vk_return_type<VkPipelineLayout> create_pipeline_layout( ) const noexcept;
    
        const vk_return_type<VkPipeline> create_graphics_pipeline(std::uint32_t stage_count,
            const VkPipelineShaderStageCreateInfo* p_stages ) const noexcept;
    
        
        
        bool check_instance_extension_support( const std::vector<const char*>& instance_extensions ) const noexcept;
    
        bool check_debug_layer_support( const std::vector<const char*>& debug_layers ) const noexcept;
    
        bool is_physical_device_suitable( const VkSurfaceKHR& surface, const VkPhysicalDevice& physical_device,
            const std::vector<const char*>& device_extensions ) const noexcept;
    
        bool check_physical_device_extension_support( const VkPhysicalDevice& physical_device,
            const std::vector<const char*>& device_extensions ) const noexcept;
    
        bool is_swapchain_adequate( const VkSurfaceKHR& surface,
            const VkPhysicalDevice& physical_device ) const noexcept;
    
        const queue_family_indices_type find_queue_family_indices( const VkSurfaceKHR& surface,
            const VkPhysicalDevice& physical_device ) const noexcept;

        const swapchain_support_details_type query_swapchain_support( const VkSurfaceKHR& surface,
                const VkPhysicalDevice& physical_device ) const noexcept;

        const VkSurfaceFormatKHR choose_swapchain_surface_format(
                const std::vector<VkSurfaceFormatKHR>& available_formats ) const noexcept;

        const VkPresentModeKHR choose_swapchain_present_mode(
                const std::vector<VkPresentModeKHR>& available_present_modes ) const noexcept;

        const VkExtent2D choose_swapchain_extent( const VkSurfaceCapabilitiesKHR& capabilities ) const noexcept;

    private:
        uint32_t window_width_;
        uint32_t window_height_;
        
        size_t current_frame_ = 0;
        
        struct vk_context_t
        {
            VkInstance instance_ = VK_NULL_HANDLE;
            VkDebugReportCallbackEXT  debug_report_ = VK_NULL_HANDLE;
            VkSurfaceKHR surface_ = VK_NULL_HANDLE;
            VkPhysicalDevice gpu_ = VK_NULL_HANDLE;
            VkDevice device_ = VK_NULL_HANDLE;
            VkQueue graphics_queue_ = VK_NULL_HANDLE;
            VkQueue present_queue_ = VK_NULL_HANDLE;
            
            std::vector<VkSemaphore> image_available_semaphores_;
            std::vector<VkSemaphore> render_finished_semaphores_;
            std::vector<VkFence> in_flight_fences_;
            
            VkCommandPool command_pool_ = VK_NULL_HANDLE;
            std::vector<VkCommandBuffer> command_buffers_ = { };
            
            VkSurfaceFormatKHR surface_format_ = { };
            
            VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
            VkExtent2D swapchain_extent_ = { };
            std::vector<VkImage> swapchain_image_ = { };
            std::vector<VkImageView> swapchain_image_views_ = { };
            std::vector<VkFramebuffer> swapchain_framebuffers_ = { };
            
            VkRenderPass render_pass_ = VK_NULL_HANDLE;
    
            VkPipelineLayout graphics_pipeline_layout_ = VK_NULL_HANDLE;
            VkPipeline graphics_pipeline_ = VK_NULL_HANDLE;
            
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
            
            bool is_complete( )
            {
                return graphic_family_.has_value() && compute_family_.has_value() && present_family_.has_value();
            }
        };
        struct swapchain_support_details_type
        {
            VkSurfaceCapabilitiesKHR capabilities_;
            std::vector<VkSurfaceFormatKHR> formats_;
            std::vector<VkPresentModeKHR> present_modes_;
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