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
    context( const window& wnd );
    context( const context& other ) = delete;
    context( context&& other );
    ~context( );

    context& operator=( const context& rhs ) = delete;
    context& operator=( context&& rhs );

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
    VkInstance instance_                        = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger_   = VK_NULL_HANDLE;
    VkSurfaceKHR surface_                       = VK_NULL_HANDLE;
    VkPhysicalDevice gpu_                       = VK_NULL_HANDLE;
    VkDevice device_                            = VK_NULL_HANDLE;

    std::vector<queue> queues_;

    std::vector<command_pool> command_pools_;

    std::vector<layer> validation_layers_;
    std::vector<extension> instance_extensions_;
    std::vector<extension> device_extensions_;
};

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

    std::vector<VkCommandBuffer> command_buffers_ = { };
};

#endif // LUCIOLE_CONTEXT_HPP