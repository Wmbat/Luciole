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

#ifndef LUCIOLE_APPLICATION_HPP
#define LUCIOLE_APPLICATION_HPP

#include <memory>
#include <unordered_map>

#include <vulkan/vulkan.h>
 
#include "graphics/renderer.hpp"

#include "window/window.hpp"

#include "strong_types.hpp"

#include "context.hpp"

struct queue;
struct command_pool;

class application
{
public:
    application( );
    ~application( );

    void run( );

private:
    std::vector<extension> get_instance_extensions( ) const;
    std::optional<std::vector<const char*>> check_instance_extension_support( ) const;

    VkInstance create_instance( const VkApplicationInfo& app_info, const std::vector<const char*>& enabled_extensions ) const;
    void destroy_instance( );

    void create_debug_messenger( );
    void destroy_debug_messenger( );

    VkSurfaceKHR create_surface( ) const;
    void destroy_surface( );

    void pick_gpu( );
    int rate_gpu( const VkPhysicalDevice gpu );

    void create_device( );
    void destroy_device( );

    std::vector<command_pool> create_command_pools( ) const;
    void destroy_command_pools( );

    std::vector<VkCommandBuffer> create_command_buffers( count32_t count ) const;

    void create_swapchain( );
    void destroy_swapchain( );

    std::vector<VkImageView> create_image_views( count32_t count ) const;
    void destroy_image_views( );

    VkRenderPass create_render_pass( ) const;
    void destroy_render_pass( );

    void create_graphics_pipeline( );
    void destroy_graphics_pipeline( );

    void create_framebuffers( );
    void destroy_framebuffers( );

    VkShaderModule create_shader_module( const std::string& code ) const;

    VkSurfaceFormatKHR choose_swapchain_surface_format( );
    VkPresentModeKHR choose_swapchain_present_mode( );
    VkExtent2D choose_swapchain_extent( const VkSurfaceCapabilitiesKHR& capabilities );

private: 
    std::unique_ptr<window> p_wnd_;

    context context_;
    renderer renderer_;

    VkInstance instance_ = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    VkPhysicalDevice gpu_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;

    std::vector<queue> queues_ = { };

    std::vector<command_pool> command_pools_ = { };

    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    std::vector<VkImage> swapchain_images_ = { };
    std::vector<VkImageView> swapchain_image_views_ = { };
    std::vector<VkFramebuffer> swapchain_framebuffers_ = { };

    VkFormat swapchain_image_format_;
    VkExtent2D swapchain_extent_;

    VkRenderPass render_pass_ = VK_NULL_HANDLE;
    VkPipeline graphics_pipeline_ = VK_NULL_HANDLE;
    VkPipelineLayout graphics_pipeline_layout_ = VK_NULL_HANDLE;

    const std::vector<const char*> validation_layers_ = { "VK_LAYER_KHRONOS_validation" };

    std::vector<extension> instance_extensions_;
    std::vector<extension> device_extensions_ =
    {
        extension{ .priority_ = extension::priority::e_required, .found_ = false, .name_ = "VK_KHR_swapchain" }
    };
};

#endif // LUCIOLE_APPLICATION_HPP