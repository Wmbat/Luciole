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

#include <luciole/luciole_core.hpp>
#include <luciole/ui/window.hpp>
#include <luciole/vk/core.hpp>
#include <luciole/vk/errors.hpp>
#include <luciole/vk/extension.hpp>
#include <luciole/vk/layer.hpp>
#include <luciole/vk/queue.hpp>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <cstdint>
#include <vector>
#include <optional>
#include <unordered_map>
#include <variant>

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
    struct command_pool
    {
        VkCommandPool handle_ = VK_NULL_HANDLE;
        queue::flag flags_ = queue::flag::e_none;
    };

    struct extensions_parameter{ };
    using extensions_t = strong_type<std::vector<vk::extension>, extensions_parameter>;

    struct extension_names_parameter{ };
    using extension_names_t = strong_type<std::vector<std::string>, extension_names_parameter>;

    struct layers_parameter{ };
    using layers_t = strong_type<std::vector<vk::layer>, layers_parameter>;

    struct layer_names_parameter{ };
    using layer_names_t = strong_type<std::vector<std::string>, layer_names_parameter>;

    struct queue_properties_parameter{ };
    using queue_properties_t = strong_type<std::vector<VkQueueFamilyProperties>, queue_properties_parameter>;

    using command_pools_container_t = std::unordered_map<std::uint32_t, context::command_pool>;

public:
    context( ) = default;
    explicit context( const ui::window& wnd );
    context( const context& other ) = delete;
    context( context&& other );
    ~context( );

    context& operator=( context const& rhs ) = delete;
    context& operator=( context&& rhs );
    
    [[nodiscard]] VkSwapchainCreateInfoKHR swapchain_create_info( ) LCL_PURE;
    
    [[nodiscard]] vk::error_variant<VkSwapchainKHR> create_swapchain( vk::swapchain_create_info_t const &create_info ) LCL_PURE;

    void destroy_swapchain( vk::swapchain_t swapchain ) const noexcept;
    
    [[nodiscard]] std::variant<VkImageView, vk::error::type> create_image_view( vk::image_view_create_info_t const& create_info ) const noexcept;
    void destroy_image_view( vk::image_view_t image_view ) const noexcept;
    
    [[nodiscard]] std::variant<VkRenderPass, vk::error::type> create_render_pass( vk::render_pass_create_info_t const& create_info ) const noexcept;
    void destroy_render_pass( vk::render_pass_t render_pass ) const noexcept;
    
    [[nodiscard]] std::variant<VkPipelineLayout, vk::error::type> create_pipeline_layout( vk::pipeline_layout_create_info_t const& create_info ) const noexcept;
    void destroy_pipeline_layout( vk::pipeline_layout_t pipeline_layout ) const noexcept;

    [[nodiscard]] std::variant<VkPipeline, vk::error::type> create_pipeline( vk::graphics_pipeline_create_info_t const& create_info ) const noexcept;
    [[nodiscard]] std::variant<VkPipeline, vk::error::type> create_pipeline( vk::compute_pipeline_create_info_t const& create_info ) const noexcept;
    void destroy_pipeline( vk::pipeline_t pipeline ) const noexcept;

    [[nodiscard]] VkShaderModule create_shader_module( vk::shader_module_create_info_t const& create_info ) const noexcept;
    void destroy_shader_module( vk::shader_module_t shader_module ) const noexcept;

    [[nodiscard]] std::variant<VkFramebuffer, vk::error::type> create_framebuffer( vk::framebuffer_create_info_t const& create_info ) const noexcept;
    void destroy_framebuffer( vk::framebuffer_t framebuffer ) const noexcept;

    [[nodiscard]] std::variant<VkSemaphore, vk::error::type> create_semaphore( vk::semaphore_create_info_t const& create_info ) const noexcept;
    void destroy_semaphore( vk::semaphore_t semaphore ) const noexcept;

    [[nodiscard]] std::variant<VkFence, vk::error::type> create_fence( vk::fence_create_info_t const& create_info ) const noexcept;
    void destroy_fence( vk::fence_t fence ) const noexcept;

    [[nodiscard]] std::variant<std::vector<VkCommandBuffer>, vk::error::type> create_command_buffers( queue::flag_t flag, count32_t buffer_count ) const;

    [[nodiscard]] std::variant<std::vector<VkImage>, vk::error::type> get_swapchain_images( vk::swapchain_t swapchain, count32_t image_count ) const;
    
    [[nodiscard]] VkSurfaceCapabilitiesKHR get_surface_capabilities( ) const noexcept;
    
    [[nodiscard]] std::vector<VkSurfaceFormatKHR> get_surface_format( ) const;
    [[nodiscard]] std::vector<VkPresentModeKHR> get_present_modes( ) const;
    [[nodiscard]] VkExtent2D get_window_extent( ) const;

    [[nodiscard]] vk::error::type submit_queue( queue::flag_t flag, vk::submit_info_t const& submit_info, vk::fence_t fence ) const noexcept;
    [[nodiscard]] vk::error::type present_queue( queue::flag_t, vk::present_info_t const& present_info ) const noexcept;

    void wait_for_fence( vk::fence_t fence ) const noexcept;
    void reset_fence( vk::fence_t fence ) const noexcept;

    //TODO: Fix this
    VkDevice get( ) const
    {
        return device_;
    }

private:
    [[nodiscard]] std::vector<vk::layer> load_validation_layers( ) const;
    [[nodiscard]] std::vector<vk::extension> load_instance_extensions( ) const;
    [[nodiscard]] std::vector<vk::extension> load_device_extensions( ) const;

    [[nodiscard]] std::vector<std::string> check_layer_support( const layers_t& layers ) const;
    [[nodiscard]] std::vector<std::string> check_ext_support( const extensions_t& extensions ) const;

    [[nodiscard]] std::variant<VkInstance, vk::error::type> create_instance( 
        VkApplicationInfo const& app_info, 
        extension_names_t const& enabled_ext_name, 
        layer_names_t const& enabled_layer_names 
    ) const;

    [[nodiscard]] std::variant<VkDebugUtilsMessengerEXT, vk::error::type> create_debug_messenger( ) const;
    [[nodiscard]] std::variant<VkSurfaceKHR, vk::error::type> create_surface( ui::window const& wnd ) const;
    [[nodiscard]] std::variant<VkPhysicalDevice, vk::error::type> pick_gpu( ) const;
    
    [[nodiscard]] std::variant<VkDevice, vk::error::type> create_device( 
        extension_names_t const& enabled_ext_name, 
        queue_properties_t const& queue_properties 
    ) const; 

    [[nodiscard]] std::unordered_map<queue::flag, queue> get_queues( const queue_properties_t& queue_properties ) const;
    [[nodiscard]] std::variant<command_pools_container_t, vk::error::type> create_command_pools( ) const;

    int rate_gpu( const VkPhysicalDevice gpu ) const;

    std::vector<VkQueueFamilyProperties> query_queue_family_properties( ) const;

private:
    glm::u32vec2 wnd_size_;
    
    VkInstance instance_ = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    VkPhysicalDevice gpu_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;

    std::unordered_map<queue::flag, queue> queues_;
    std::unordered_map<std::uint32_t, command_pool> command_pools_;

    std::vector<vk::layer> validation_layers_;
    std::vector<vk::extension> instance_extensions_;
    std::vector<vk::extension> device_extensions_;
};

using p_context_t = strong_type<context const*>;

#endif // LUCIOLE_CONTEXT_HPP