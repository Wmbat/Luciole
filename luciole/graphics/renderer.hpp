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

using p_context_t = strong_type<context const*>;

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

private:
    VkSwapchainKHR create_swapchain( VkSurfaceCapabilitiesKHR const& capabilities, VkSurfaceFormatKHR const& format ) const;
    std::vector<VkImageView> create_image_views( count32_t image_count ) const;
    VkRenderPass create_render_pass( ) const;
    
    VkShaderModule create_shader_module( shader_filepath_t filepath );
    
    VkSurfaceFormatKHR pick_swapchain_format( ) const;
    VkPresentModeKHR pick_swapchain_present_mode( ) const;
    VkExtent2D pick_swapchain_extent( VkSurfaceCapabilitiesKHR const& capabilities ) const;
    
private:
    const context* p_context_;
    
    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    std::vector<VkImage> swapchain_images_ = { };
    std::vector<VkImageView> swapchain_image_views_ = { };
    std::vector<VkFramebuffer> swapchain_framebuffers_ = { };

    VkFormat swapchain_image_format_;
    VkExtent2D swapchain_extent_;

    VkRenderPass render_pass_ = VK_NULL_HANDLE;
    VkPipeline graphics_pipeline_ = VK_NULL_HANDLE;
    VkPipelineLayout graphics_pipeline_layout_ = VK_NULL_HANDLE;
};

#endif // LUCIOLE_GRAPHICS_RENDERER_HPP