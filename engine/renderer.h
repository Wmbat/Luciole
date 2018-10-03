/*!
 *  Copyright (C) 2018 BouwnLaw
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

namespace engine
{
    class renderer
    {
    private:
        /// Helper structs ///
        struct queue_family_indices;
        struct swapchain_support_details;
        //////////////////////

    public:
        renderer( const engine::window& window, const std::string& app_name, uint32_t app_version );

        ~renderer( );

        void setup_graphics_pipeline( const std::string& vert_shader_filepath, const std::string& frag_shader_filepath );

        void record_command_buffers( ) const noexcept;
        void draw_frame( );

    private:
        const vk::Instance create_instance( const std::vector<const char*>& extensions,
                                            const std::vector<const char*>& validation_layers,
                                            const std::string& app_name, uint32_t app_version ) const noexcept;
#ifndef NDEBUG
        const vk::DebugReportCallbackEXT create_debug_report_callback( const vk::Instance& instance_handle ) const noexcept;
#endif
        const vk::SurfaceKHR create_surface( const window& wnd, const vk::Instance& instance_handle ) const noexcept;
        const vk::PhysicalDevice pick_physical_device( const std::vector<const char*>& device_extensions,
                                                       const vk::Instance& instance_handle ) const noexcept;
        const vk::Device create_logical_device( const std::vector<const char*>& validation_layers,
                                                const std::vector<const char*>& device_extensions,
                                                const vk::PhysicalDevice& physical_device_handle ) const noexcept;
        const vk::Semaphore create_semaphore( const vk::Device& logical_device_handle ) const noexcept;
        const vk::SwapchainKHR create_swapchain( const vk::Device& logical_device_handle,
                                                 const vk::PhysicalDevice& physical_device_handle,
                                                 const swapchain_support_details& swapchain_support_details,
                                                 const vk::SurfaceFormatKHR& surface_format, const vk::Extent2D& extent_2d,
                                                 const vk::PresentModeKHR& present_mode,
                                                 uint32_t image_count ) const noexcept;
        const std::vector<vk::ImageView> create_swapchain_image_views( const vk::Device& logical_device_handle,
                                                                       const vk::Format& swapchain_image_format,
                                                                       const std::vector<vk::Image>& swapchain_image_handles,
                                                                       uint32_t image_view_count ) const noexcept;
        const vk::ShaderModule create_shader_module( const vk::Device& logical_device_handle,
                                                     const std::string& shader_code ) const noexcept;
        const vk::RenderPass create_render_pass( const vk::Device& logical_device_handle,
                                                 const vk::Format& swapchain_image_format ) const noexcept;
        const vk::CommandPool create_command_pool( const vk::Device& logical_device_handle,
                                                   const queue_family_indices& queue_family_indices ) const noexcept;
        const std::vector<vk::CommandBuffer> allocate_command_buffers( const vk::Device& logical_device_handle,
                                                                       const vk::CommandPool& command_pool_handle,
                                                                       uint32_t buffer_count ) const noexcept;
        const std::vector<vk::Framebuffer> create_swapchain_framebuffers( const vk::Device& logical_device_handle,
                                                                          const vk::RenderPass& render_pass_handle,
                                                                          const vk::Extent2D& swapchain_image_extent_2d,
                                                                          const std::vector<vk::ImageView>& swapchain_image_view_handles,
                                                                          uint32_t framebuffer_count ) const noexcept;
        const vk::PipelineLayout create_graphics_pipeline_layout( const vk::Device& logical_device_handle ) const noexcept;
        const vk::Pipeline create_graphics_pipeline( const vk::Device& logical_device_handle,
                                                     const vk::Extent2D& swapchain_image_extent_2d,
                                                     const vk::PipelineLayout& graphics_pipeline_layout_handle,
                                                     const vk::RenderPass& render_pass_handle,
                                                     const vk::ShaderModule& vert_shader_handle,
                                                     const vk::ShaderModule& frag_shader_handle ) const noexcept;


        /// Helper Funcs ///
        bool check_extensions_support( const std::vector<const char*>& extensions ) const noexcept;
        bool check_physical_device_extension_support( const vk::PhysicalDevice& physical_device_handle,
                                                      const std::vector<const char*>& device_extensions ) const noexcept;
        bool check_validation_layer_support( const std::vector<const char*>& validation_layers ) const noexcept;

        int rate_physical_device( const vk::PhysicalDevice& physical_device_handle ) const noexcept;

        bool is_physical_device_suitable( const vk::PhysicalDevice& physical_device_handle,
                                          const std::vector<const char*>& device_extensions ) const noexcept;
        bool is_swapchain_adequate( const vk::PhysicalDevice& physical_device_handle ) const noexcept;

        queue_family_indices find_queue_families( const vk::PhysicalDevice& physical_device_handle ) const noexcept;

        vk::SurfaceFormatKHR choose_swapchain_surface_format( const std::vector<vk::SurfaceFormatKHR>& available_formats ) const noexcept;
        vk::PresentModeKHR choose_swapchain_present_mode( const std::vector<vk::PresentModeKHR>& available_formats ) const noexcept;
        vk::Extent2D choose_swapchain_extent_2d( const vk::SurfaceCapabilitiesKHR& capabilities ) const noexcept;
        ////////////////////

    private:
        vk::Instance instance_handle_;
#ifndef NDEBUG
        vk::DebugReportCallbackEXT debug_report_callback_handle_;
#endif
        vk::SurfaceKHR surface_handle_;
        vk::PhysicalDevice physical_device_handle_;
        vk::Device logical_device_handle_;
        vk::Queue graphics_queue_;
        vk::Queue present_queue_;

        vk::Semaphore image_available_semaphore_handle_;
        vk::Semaphore render_finished_semaphore_handle_;

        vk::SwapchainKHR swapchain_handle_;
        vk::Format swapchain_image_format_;
        vk::Extent2D swapchain_image_extent_2d_;
        std::vector<vk::Image> swapchain_image_handles_;
        std::vector<vk::ImageView> swapchain_image_view_handles_;
        std::vector<vk::Framebuffer> swapchain_framebuffer_handles_;

        vk::RenderPass render_pass_handle_;

        vk::CommandPool command_pool_handle_;
        std::vector<vk::CommandBuffer> command_buffer_handles_;

        vk::PipelineLayout graphics_pipeline_layout_handle_;
        vk::Pipeline graphics_pipeline_handle_;

        uint32_t window_width_;
        uint32_t window_height_;

    private:
        /// Helper structs ///
        struct queue_family_indices
        {
            int32_t graphics_family_ = -1;
            int32_t present_family_ = -1;
            int32_t compute_family_ = -1;

            bool is_complete( )
            {
                return ( graphics_family_ >= 0 ) && ( compute_family_ >= 0 ) && ( present_family_ >= 0 );
            }
        };
        struct swapchain_support_details
        {
            vk::SurfaceCapabilitiesKHR capabilities_;
            std::vector<vk::SurfaceFormatKHR> formats_;
            std::vector<vk::PresentModeKHR> present_modes_;
        };
        //////////////////////
    };
}

#endif //VULKAN_PROJECT_RENDERER_H