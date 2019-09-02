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


#ifndef LUCIOLE_LUCIOLE_CORE_HPP
#define LUCIOLE_LUCIOLE_CORE_HPP

#include <cstdint>
#include <optional>
#include <string>

#include <vulkan/vulkan.h>

#if defined( BUILD_TEST )
#include <gtest/gtest.h>
#endif

#include "utilities/log.hpp"

#include "strong_types.hpp"

namespace vk
{
    struct default_param{ }; 

    using instance_t = strong_type<VkInstance, default_param>;
    using debug_messenger_t = strong_type<VkDebugUtilsMessengerEXT, default_param>;
    using surface_t = strong_type<VkSurfaceKHR, default_param>;
    using physical_device_t = strong_type<VkPhysicalDevice, default_param>;
    using device_t = strong_type<VkDevice, default_param>;
    using queue_t = strong_type<VkQueue, default_param>;
    using submit_info_t = strong_type<VkSubmitInfo, default_param>;
    using present_info_t = strong_type<VkPresentInfoKHR, default_param>;
    using swapchain_t = strong_type<VkSwapchainKHR, default_param>;
    using swapchain_create_info_t = strong_type<VkSwapchainCreateInfoKHR, default_param>;
    using image_t = strong_type<VkImage, default_param>;
    using image_view_t = strong_type<VkImageView, default_param>;
    using image_view_create_info_t = strong_type<VkImageViewCreateInfo, default_param>;
    using render_pass_t = strong_type<VkRenderPass, default_param>;
    using render_pass_create_info_t = strong_type<VkRenderPassCreateInfo, default_param>;
    using pipeline_layout_t = strong_type<VkPipelineLayout, default_param>;
    using pipeline_layout_create_info_t = strong_type<VkPipelineLayoutCreateInfo, default_param>;
    using pipeline_t = strong_type<VkPipeline, default_param>;
    using graphics_pipeline_create_info_t = strong_type<VkGraphicsPipelineCreateInfo, default_param>;
    using compute_pipeline_create_info_t = strong_type<VkComputePipelineCreateInfo, default_param>;
    using shader_module_t = strong_type<VkShaderModule, default_param>;
    using shader_module_create_info_t = strong_type<VkShaderModuleCreateInfo, default_param>;
    using framebuffer_t = strong_type<VkFramebuffer, default_param>;
    using framebuffer_create_info_t = strong_type<VkFramebufferCreateInfo, default_param>;
    using semaphore_t = strong_type<VkSemaphore, default_param>;
    using semaphore_create_info_t = strong_type<VkSemaphoreCreateInfo, default_param>;
    using fence_t = strong_type<VkFence, default_param>;
    using fence_create_info_t = strong_type<VkFenceCreateInfo, default_param>;
}


constexpr auto cache_line = std::size_t{ 64 };

static constexpr uint32_t kilobyte = 1024;
static constexpr uint32_t megabyte = kilobyte * kilobyte;
    
constexpr unsigned long long operator "" _kg( unsigned long long size ) { return size * kilobyte; }
constexpr unsigned long long operator "" _mb( unsigned long long size ) { return size * megabyte; }

#endif //LUCIOLE_LUCIOLE_CORE_HPP