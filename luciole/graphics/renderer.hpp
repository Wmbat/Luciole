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

class renderer
{
public:

private:
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