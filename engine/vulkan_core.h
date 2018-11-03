/*!
 *  Copyright (C) 2018 Wmbat
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

#ifndef VULKAN_PROJECT_VULKAN_CORE_H
#define VULKAN_PROJECT_VULKAN_CORE_H

#include <optional>

#include <vulkan/vulkan.h>

#include "vulkan_utils.h"
#include "window.h"

namespace engine
{
    class vulkan_core
    {
    public:
        struct queue_family_indices;

    public:
        vulkan_core( const window& wnd, const std::string& app_name, uint32_t app_version );
        vulkan_core( const vulkan_core& rhs ) noexcept = delete;
        vulkan_core( vulkan_core&& rhs ) noexcept;
        ~vulkan_core( );

        vulkan_core& operator=( const vulkan_core& rhs ) noexcept = delete;
        vulkan_core& operator=( vulkan_core&& rhs ) noexcept;

        const VkInstance& get_instance( ) const noexcept;
        const VkSurfaceKHR& get_surface( ) const noexcept;
        const VkPhysicalDevice& get_physical_device( ) const noexcept;
        const VkDevice& get_device( ) const noexcept;

    private:
        const vk_return_obj<VkInstance> create_instance( const std::vector<const char*>& extensions,
                                                         const std::vector<const char*>& debug_layers,
                                                         const std::string& app_name, uint32_t app_version ) const noexcept;

        const vk_return_obj<VkDebugReportCallbackEXT> create_debug_report( const VkInstance& instance ) const noexcept;

        const vk_return_obj<VkSurfaceKHR> create_surface( const window& wnd, const VkInstance& instance ) const noexcept;

        const vk_return_obj<VkPhysicalDevice> pick_physical_device( const VkSurfaceKHR& surface,
                const std::vector<const char*>& device_extensions ) const noexcept;

        const vk_return_obj<VkDevice> create_device( ) const noexcept;


        bool check_instance_extension_support( const std::vector<const char*>& instance_extensions ) const noexcept;

        bool check_debug_layer_support( const std::vector<const char*>& debug_layers ) const noexcept;

        bool is_physical_device_suitable( const VkSurfaceKHR& surface, const VkPhysicalDevice& physical_device,
                                          const std::vector<const char*>& device_extensions ) const noexcept;





        bool check_physical_device_extension_support( const VkPhysicalDevice& physical_device,
                                                      const std::vector<const char*>& device_extensions ) const noexcept;

        bool is_swapchain_adequate( const VkSurfaceKHR& surface,
                                    const VkPhysicalDevice& physical_device ) const noexcept;

        const queue_family_indices find_queue_family_indices( const VkSurfaceKHR& surface,
                                                              const VkPhysicalDevice& physical_device ) const noexcept;


    private:
        VkInstance instance_ = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT  debug_report_ = VK_NULL_HANDLE;
        VkSurfaceKHR surface_ = VK_NULL_HANDLE;
        VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
        VkDevice device_ = VK_NULL_HANDLE;


    public:
        struct queue_family_indices
        {
            std::optional<uint32_t> graphic_family_;
            std::optional<uint32_t> compute_family_;
            std::optional<uint32_t> present_family_;

            bool is_complete( )
            {
                return graphic_family_.has_value() && compute_family_.has_value() && present_family_.has_value();
            }
        };
    };
}

#endif //VULKAN_PROJECT_VULKAN_CORE_H