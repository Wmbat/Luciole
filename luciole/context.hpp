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

#ifndef LUCIOLE_CONTEXT_HPP
#define LUCIOLE_CONTEXT_HPP

#include <cstdint>
#include <vector>
#include <optional>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "window/window.hpp"

#include "extension.hpp"
#include "layer.hpp"
#include "strong_types.hpp"

#if defined( NDEBUG )
    static constexpr bool enable_debug_layers = false;
#else
    static constexpr bool enable_debug_layers = true;
#endif

struct queue;
struct command_pool;

class context
{
private:
    struct queue
    {
        VkQueue handle_ = VK_NULL_HANDLE;
        VkQueueFlags flags_ = 0;
        std::uint32_t family_ = 0;
        std::uint32_t index_ = 0;
    };

    struct command_pool
    {
        VkCommandPool handle_ = VK_NULL_HANDLE;
        std::uint32_t family_ = 0;
        VkQueueFlags flags_ = 0;
    };

    struct extensions_parameter{ };
    using extensions_t = strong_type<std::vector<extension>, extensions_parameter>;

    struct extension_names_parameter{ };
    using extension_names_t = strong_type<std::vector<std::string>, extension_names_parameter>;

    struct layers_parameter{ };
    using layers_t = strong_type<std::vector<layer>, layers_parameter>;

    struct layer_names_parameter{ };
    using layer_names_t = strong_type<std::vector<std::string>, layer_names_parameter>;

    struct queue_properties_parameter{ };
    using queue_properties_t = strong_type<std::vector<VkQueueFamilyProperties>, queue_properties_parameter>;

public:
    context( ) = default;
    explicit context( const window& wnd );
    context( const context& other ) = delete;
    context( context&& other );
    ~context( );

    context& operator=( context const& rhs ) = delete;
    context& operator=( context&& rhs );
    
    [[nodiscard]] VkSwapchainCreateInfoKHR swapchain_create_info( ) const noexcept;
    
    [[nodiscard]] VkSwapchainKHR create_swapchain( vk_swapchain_create_info_t create_info ) const noexcept;
    void destroy_swapchain( VkSwapchainKHR swapchain ) const noexcept;
    
    [[nodiscard]] VkImageView create_image_view( vk_image_view_create_info_t create_info ) const noexcept;
    void destroy_image_view( vk_image_view_t image_view ) const noexcept;
    
    [[nodiscard]] VkRenderPass create_render_pass( vk_render_pass_create_info_t create_info ) const noexcept;
    void destroy_render_pass( vk_render_pass_t render_pass ) const noexcept;
    
    [[nodiscard]] VkPipelineLayout create_pipeline_layout( vk_pipeline_layout_create_info_t create_info ) const noexcept;
    void destroy_pipeline_layout( vk_pipeline_layout_t pipeline_layout ) const noexcept;

    [[nodiscard]] VkPipeline create_pipeline( vk_graphics_pipeline_create_info_t create_info ) const noexcept;
    [[nodiscard]] VkPipeline create_pipeline( vk_compute_pipeline_create_info_t create_info ) const noexcept;
    void destroy_pipeline( vk_pipeline_t pipeline ) const noexcept;

    [[nodiscard]] VkShaderModule create_shader_module( vk_shader_module_create_info_t create_info ) const noexcept;
    void destroy_shader_module( vk_shader_module_t shader_module ) const noexcept;

    [[nodiscard]] VkFramebuffer create_framebuffer( vk_framebuffer_create_info_t create_info ) const noexcept;
    void destroy_framebuffer( vk_framebuffer_t framebuffer ) const noexcept;

    [[nodiscard]] VkSemaphore create_semaphore( vk_semaphore_create_info_t create_info ) const noexcept;
    void destroy_semaphore( vk_semaphore_t semaphore ) const noexcept;

    [[nodiscard]] VkFence create_fence( vk_fence_create_info_t create_info ) const noexcept;
    void destroy_fence( vk_fence_t fence ) const noexcept;

    [[nodiscard]] std::vector<VkCommandBuffer> create_command_buffers( VkQueueFlags flag, count32_t buffer_count ) const;

    [[nodiscard]] std::vector<VkImage> get_swapchain_images( vk_swapchain_t swapchain, count32_t image_count ) const;
    
    [[nodiscard]] VkSurfaceCapabilitiesKHR get_surface_capabilities( ) const noexcept;
    
    [[nodiscard]] std::vector<VkSurfaceFormatKHR> get_surface_format( ) const;
    [[nodiscard]] std::vector<VkPresentModeKHR> get_present_modes( ) const;
    [[nodiscard]] VkExtent2D get_window_extent( ) const;

    VkResult submit_queue( VkQueueFlags flag, vk_submit_info_t submit_info, vk_fence_t fence ) const noexcept;
    void present_queue( VkQueueFlags flag, vk_present_info_t present_info ) const noexcept;

    void wait_for_fence( vk_fence_t fence ) const noexcept;
    void reset_fence( vk_fence_t fence ) const noexcept;

    //TODO: Fix this
    VkDevice get( ) const
    {
        return device_;
    }

private:
    std::vector<layer> load_validation_layers( ) const;
    std::vector<extension> load_instance_extensions( ) const;
    std::vector<extension> load_device_extensions( ) const;

    std::vector<std::string> check_layer_support( const layers_t& layers ) const;
    std::vector<std::string> check_ext_support( const extensions_t& extensions ) const;

    VkInstance create_instance( const VkApplicationInfo& app_info, const extension_names_t& enabled_ext_name, const layer_names_t& enabled_layer_names ) const;
    VkDebugUtilsMessengerEXT create_debug_messenger( ) const;
    VkSurfaceKHR create_surface( const window& wnd ) const;
    VkPhysicalDevice pick_gpu( ) const;
    VkDevice create_device( const extension_names_t& enabled_ext_name, const queue_properties_t& queue_properties ) const; 


    std::vector<queue> get_queues( const queue_properties_t& queue_properties ) const;

    std::vector<command_pool> create_command_pools( ) const;

    int rate_gpu( const VkPhysicalDevice gpu ) const;

    std::vector<VkQueueFamilyProperties> query_queue_family_properties( ) const;

private:
    glm::u32vec2 wnd_size_;
    
    VkInstance instance_ = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    VkPhysicalDevice gpu_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;

    std::vector<queue> queues_;

    std::vector<command_pool> command_pools_;

    std::vector<layer> validation_layers_;
    std::vector<extension> instance_extensions_;
    std::vector<extension> device_extensions_;
};

using p_context_t = strong_type<context const*>;

#endif // LUCIOLE_CONTEXT_HPP