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

struct vulkan_default_parameter { };

template<typename T>
using vk_strong_type = strong_type<T, vulkan_default_parameter>;

using vk_instance_t = vk_strong_type<VkInstance>;
using vk_debug_messenger_t = vk_strong_type<VkDebugUtilsMessengerEXT>;
using vk_surface_t = vk_strong_type<VkSurfaceKHR>;
using vk_physical_device_t = vk_strong_type<VkPhysicalDevice>;
using vk_device_t = vk_strong_type<VkDevice>;
using vk_queue_t = vk_strong_type<VkQueue>;
using vk_submit_info_t = vk_strong_type<VkSubmitInfo const&>;
using vk_present_info_t = vk_strong_type<VkPresentInfoKHR const&>;
using vk_swapchain_t = vk_strong_type<VkSwapchainKHR>;
using vk_swapchain_create_info_t = vk_strong_type<VkSwapchainCreateInfoKHR const&>;
using vk_image_view_t = vk_strong_type<VkImageView>;
using vk_image_view_create_info_t = strong_type<VkImageViewCreateInfo const&>;
using vk_render_pass_t = vk_strong_type<VkRenderPass>;
using vk_render_pass_create_info_t = vk_strong_type<VkRenderPassCreateInfo const&>;
using vk_pipeline_layout_t = vk_strong_type<VkPipelineLayout>;
using vk_pipeline_layout_create_info_t = vk_strong_type<VkPipelineLayoutCreateInfo const&>;
using vk_pipeline_t = vk_strong_type<VkPipeline>;
using vk_graphics_pipeline_create_info_t = vk_strong_type<VkGraphicsPipelineCreateInfo const&>;
using vk_compute_pipeline_create_info_t = vk_strong_type<VkComputePipelineCreateInfo const&>;
using vk_shader_module_t = vk_strong_type<VkShaderModule>;
using vk_shader_module_create_info_t = vk_strong_type<VkShaderModuleCreateInfo const&>;
using vk_framebuffer_t = vk_strong_type<VkFramebuffer>;
using vk_framebuffer_create_info_t = vk_strong_type<VkFramebufferCreateInfo const&>;
using vk_semaphore_t = vk_strong_type<VkSemaphore>;
using vk_semaphore_create_info_t = vk_strong_type<VkSemaphoreCreateInfo const&>;
using vk_fence_t = vk_strong_type<VkFence>;
using vk_fence_create_info_t = vk_strong_type<VkFenceCreateInfo const&>;

constexpr auto cache_line = std::size_t{ 64 };

struct error_msg_parameter { };
using error_msg_t = strong_type<std::string, error_msg_parameter>;

template<typename T>
T vk_check( vk_strong_type<T> handle, const error_msg_t& err_msg )
{
    if ( handle.value_ == VK_NULL_HANDLE )
    {
        core_error( err_msg.value_ );
        throw;
    }

    return handle.value_;
}

template<typename T>
std::vector<T> vk_check_array( const std::vector<T>& handles, const error_msg_t& err_msg )
{
    if ( handles.size( ) == 0 )
    {
        core_error( err_msg.value_ );
        throw;
    }

    return handles;
}

static constexpr uint32_t kilobyte = 1024;
static constexpr uint32_t megabyte = kilobyte * kilobyte;
    
constexpr unsigned long long operator "" _kg( unsigned long long size ) { return size * kilobyte; }
constexpr unsigned long long operator "" _mb( unsigned long long size ) { return size * megabyte; }

#endif //LUCIOLE_LUCIOLE_CORE_HPP