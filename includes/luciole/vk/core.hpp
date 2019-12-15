/**
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

#ifndef LUCIOLE_VULKAN_CORE_HPP
#define LUCIOLE_VULKAN_CORE_HPP

/* INCLUDES */
#include <luciole/utils/strong_types.hpp>

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <variant>

namespace vk
{
#if defined( NDEBUG )
   static constexpr bool enable_debug_layers = false;
#else
   static constexpr bool enable_debug_layers = true;
#endif

   struct default_param
   {
   };

   using instance_t = strong_type<VkInstance, default_param>;
   using debug_messenger_t = strong_type<VkDebugUtilsMessengerEXT, default_param>;
   using surface_t = strong_type<VkSurfaceKHR, default_param>;
   using physical_device_t = strong_type<VkPhysicalDevice, default_param>;
   using device_t = strong_type<VkDevice, default_param>;
   using queue_t = strong_type<VkQueue, default_param>;
   using submit_info_t = strong_type<VkSubmitInfo const&, default_param>;
   using present_info_t = strong_type<VkPresentInfoKHR const&, default_param>;
   using swapchain_t = strong_type<VkSwapchainKHR, default_param>;
   using swapchain_create_info_t = strong_type<VkSwapchainCreateInfoKHR const&, default_param>;
   using image_t = strong_type<VkImage, default_param>;
   using image_view_t = strong_type<VkImageView, default_param>;
   using image_view_create_info_t = strong_type<VkImageViewCreateInfo const&, default_param>;
   using render_pass_t = strong_type<VkRenderPass, default_param>;
   using render_pass_create_info_t = strong_type<VkRenderPassCreateInfo const&, default_param>;
   using descriptor_pool_t = strong_type<VkDescriptorPool, default_param>;
   using descriptor_pool_create_info_t = strong_type<VkDescriptorPoolCreateInfo const&, default_param>;
   using pipeline_layout_t = strong_type<VkPipelineLayout, default_param>;
   using pipeline_layout_create_info_t = strong_type<VkPipelineLayoutCreateInfo const&, default_param>;
   using descriptor_set_layout_t = strong_type<VkDescriptorSetLayout const&, default_param>;
   using descriptor_set_layout_create_info_t = strong_type<VkDescriptorSetLayoutCreateInfo const&, default_param>;
   using pipeline_t = strong_type<VkPipeline, default_param>;
   using graphics_pipeline_create_info_t = strong_type<VkGraphicsPipelineCreateInfo const&, default_param>;
   using compute_pipeline_create_info_t = strong_type<VkComputePipelineCreateInfo const&, default_param>;
   using shader_module_t = strong_type<VkShaderModule, default_param>;
   using shader_module_create_info_t = strong_type<VkShaderModuleCreateInfo const&, default_param>;
   using framebuffer_t = strong_type<VkFramebuffer, default_param>;
   using framebuffer_create_info_t = strong_type<VkFramebufferCreateInfo const&, default_param>;
   using semaphore_t = strong_type<VkSemaphore, default_param>;
   using semaphore_create_info_t = strong_type<VkSemaphoreCreateInfo const&, default_param>;
   using fence_t = strong_type<VkFence, default_param>;
   using fence_create_info_t = strong_type<VkFenceCreateInfo const&, default_param>;
   using result_t = strong_type<VkResult, default_param>;
} // namespace vk

#endif // LUCIOLE_VULKAN_CORE_HPP
