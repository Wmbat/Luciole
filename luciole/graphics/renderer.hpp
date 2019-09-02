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

#ifndef LUCIOLE_GRAPHICS_RENDERER_HPP
#define LUCIOLE_GRAPHICS_RENDERER_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "../context.hpp"

class renderer
{
private:
    struct shader_filepath_parameter{ };
    using shader_filepath_t = strong_type<std::string const&, shader_filepath_parameter>;
    
public:
    renderer( ) = default;
    explicit renderer( p_context_t p_context );
    renderer( renderer const& rhs ) = delete;
    renderer( renderer&& rhs );
    ~renderer( );
    
    renderer& operator=( renderer const& rhs ) = delete;
    renderer& operator=( renderer&& rhs );

    void draw_frame();

private:
    void record_command_buffers( );

    [[nodiscard]] std::variant<VkSwapchainKHR, vk::error::type> create_swapchain( VkSurfaceCapabilitiesKHR const& capabilities, VkSurfaceFormatKHR const& format ) const;
    [[nodiscard]] std::variant<VkImageView, vk::error::type> create_image_view( vk::image_t image ) const;
    [[nodiscard]] std::variant<VkRenderPass, vk::error::type> create_render_pass( ) const;
    [[nodiscard]] VkShaderModule create_shader_module( shader_filepath_t filepath ) const;
    [[nodiscard]] std::variant<VkPipelineLayout, vk::error::type> create_default_pipeline_layout( ) const;
    [[nodiscard]] std::variant<VkPipeline, vk::error::type> create_default_pipeline( shader_filepath_t vert_filepath, shader_filepath_t frag_filepath ) const;
    [[nodiscard]] std::variant<VkSemaphore, vk::error::type> create_semaphore( ) const;
    [[nodiscard]] std::variant<VkFence, vk::error::type> create_fence( ) const noexcept;
    [[nodiscard]] std::variant<VkFramebuffer, vk::error::type> create_framebuffer( vk::image_view_t image_view ) const noexcept;

    [[nodiscard]] VkSurfaceFormatKHR pick_swapchain_format( ) const;
    [[nodiscard]] VkPresentModeKHR pick_swapchain_present_mode( ) const;
    [[nodiscard]] VkExtent2D pick_swapchain_extent( VkSurfaceCapabilitiesKHR const& capabilities ) const;
    
private:
    static constexpr int MAX_FRAMES_IN_FLIGHT_ = 2;

    const context* p_context_;
    
    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    std::vector<VkImage> swapchain_images_ = { };
    std::vector<VkImageView> swapchain_image_views_ = { };
    std::vector<VkFramebuffer> swapchain_framebuffers_ = { };

    VkFormat swapchain_image_format_;
    VkExtent2D swapchain_extent_;

    VkRenderPass render_pass_ = VK_NULL_HANDLE;
    VkPipeline default_graphics_pipeline_ = VK_NULL_HANDLE;
    VkPipelineLayout default_graphics_pipeline_layout_ = VK_NULL_HANDLE;

    std::vector<VkCommandBuffer> render_command_buffers_ = { };

    VkSemaphore image_available_semaphore_[MAX_FRAMES_IN_FLIGHT_] = { };
    VkSemaphore render_finished_semaphore_[MAX_FRAMES_IN_FLIGHT_] = { };
    VkFence in_flight_fences[MAX_FRAMES_IN_FLIGHT_] = { };

    size_t current_frame = 0;
};

#endif // LUCIOLE_GRAPHICS_RENDERER_HPP