/*!
 * @author wmbat@protonmail.com
 *
 * Copyright (C) 2018-2019 Wmbat
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * You should have received a copy of the GNU General Public License
 * GNU General Public License for more details.
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MARSUPIAL_VULKAN_CONTEXT_HPP
#define MARSUPIAL_VULKAN_CONTEXT_HPP

/* STL */
#include <map>
#include <string_view>

/* Local */
#include "vulkan.hpp"
#include "utils.hpp"

/* General */
#include "../marsupial_core.hpp"
#include "../window/base_window.hpp"
#include "../utilities/log.hpp"

namespace marsupial::vulkan
{
    enum class queue_type
    {
        graphics,
        transfer
    };
    
    /*!
     * @name context
     * @brief A Vulkan API context class for the creation and initialization
     * of core Vulkan API objects such as VkInstance, VkDebugReportCallbackEXT,
     * VkPhysicalDevice, VkDevice and VkCommandPool.
     */
    struct context
    {
    public:
        /*!
         * @name create_info
         * @brief Struct to pass data to the context class.
         */ 
        struct create_info
        {
            create_info( base_window *p_wnd = nullptr, std::string_view app_name = { },
                std::uint32_t app_version = 0, std::uint32_t max_frames_in_flight = 1 ) noexcept;
    
            create_info& set_p_window( base_window *p_wnd ) noexcept;
            create_info& set_app_name( std::string_view app_name ) noexcept;
            create_info& set_app_version( std::uint32_t app_version ) noexcept; 
            create_info& set_max_frames_in_flight( std::uint32_t max_frames_in_flight ) noexcept;
        
            base_window *p_wnd_;
        
            std::string app_name_;
            std::uint32_t app_version_;
            
            std::uint32_t max_frames_in_flight_;
        };
        
    private:
        /*!
         * @name extension_properties_trait
         * @brief Template trait type for differentiation of extension type
         * required by the Vulkan API for VkInstance (vk::Instance), 
         * VkDebugReportCallbackEXT (vk::DebugReportCallbackEXT) and VkDevice (vk::Device).
         */
        template<class vk_object_type>
        struct extension_properties_trait;

        /*!
         * @name extension_properties_t
         * @brief Shortcut to using "extension_properties_trait" template struct trait for
         * extension types of the Vulkan API.
         */
        template<class vk_object_type>
        using extension_properties_t = typename extension_properties_trait<vk_object_type>::type;

        /*!
         * @name queue_family_info
         * @brief Helper struct for the storage of the different Vulkan
         * queue family indices.
         */
        struct queue_family_info
        {
            queue_family_info(
                const std::uint32_t queue_count = 0,
                const std::optional<std::uint32_t> graphics = std::optional<std::uint32_t>( ),
                const std::optional<std::uint32_t> present = std::optional<std::uint32_t>( ),
                const std::optional<std::uint32_t> transfer = std::optional<std::uint32_t>( ) );
            
            /* Helper functions to set the values of the struct. */
            queue_family_info& set_queue_count( std::uint32_t queue_count ) noexcept;
            queue_family_info& set_graphics( std::uint32_t index ) noexcept;
            queue_family_info& set_present( uint32_t index ) noexcept;
            queue_family_info& set_transfer( uint32_t index ) noexcept;
            
            bool has_rendering_support( ) const noexcept;
            bool is_general_purpose( ) const noexcept;
            bool is_dedicated_transfer( ) const noexcept;
            bool is_dedicated_graphics( ) const noexcept;
        
            uint32_t queue_count_;
            
            std::optional<std::uint32_t> graphics_;
            std::optional<std::uint32_t> present_;
            std::optional<std::uint32_t> transfer_;
        };
    
    public:
        context( ) = default;
        context( const create_info& create_info );            
        context( const context& rhs ) = delete;
        context( context&& rhs );
        ~context( ) = default;
        
        context& operator=( const context& rhs ) = delete;
        context& operator=( context&& rhs );
    
    private:
        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback_function( VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location,
            int32_t code, const char *layerPrefix, const char *msg, void *userData )
        {
            if ( flags & VK_DEBUG_REPORT_WARNING_BIT_EXT )
            {
                core_warn( "Validation Layers -> {0}.", msg );
            }
            else if ( flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT )
            {
                core_warn( "Validation Layers -> {0}.", msg );
            }
            else if ( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT )
            {
                core_error( "Validation Layers -> {0}.", msg );
            }
            
            return VK_FALSE;
        }
        
        vk::UniqueInstance create_instance( const std::uint32_t api_version, const std::string_view app_name, const std::uint32_t app_version ) const noexcept;
        vk::UniqueDebugReportCallbackEXT create_debug_report_callback( ) const noexcept;
        vk::PhysicalDevice pick_physical_device( ) const;
        vk::UniqueDevice create_logical_device( const std::vector<queue_family_info>& queue_family_infos ) const;
        vk::UniqueCommandPool create_command_pool( uint32_t queue_family_index ) const noexcept;
        
        bool is_gpu_suitable( const vk::PhysicalDevice gpu ) const;
        
        std::vector<queue_family_info> get_queue_family_infos( const std::vector<vk::QueueFamilyProperties>& queue_properties ) const;

        template<class vk_object_type>
        bool check_extension_support( const extension_properties_t<vk_object_type>& properties ) const;
    
    public:
        vk::UniqueInstance instance_;
        vk::UniqueDebugReportCallbackEXT debug_callback_;
        vk::UniqueSurfaceKHR surface_;
        vk::PhysicalDevice gpu_;
        vk::UniqueDevice device_;
        
        vk::Queue graphics_queue_;
        vk::Queue transfer_queue_;
        
        uint32_t queue_family_count_ = 0;
        std::vector<uint32_t> queue_family_indices_;
        
        std::vector<vk::UniqueCommandPool> graphics_command_pools_;
        vk::UniqueCommandPool transfer_command_pool_;
    
    private:
        std::vector<const char *> instance_extensions_;
        std::vector<const char *> device_extensions_;
        std::vector<const char *> validation_layers_;
    };

    /*!
     * @name constext::extension_properties_trait
     * @brief Specialization of the struct context::extension_properties_trait
     * for the VkInstance (vk::Instance) extensions.
     */
    template<>
    struct context::extension_properties_trait<vk::Instance>
    {
        using type = std::vector<vk::ExtensionProperties>;
    };

    /*!
     * @name constext::extension_properties_trait
     * @brief Specialization of the struct context::extension_properties_trait
     * for the VkDevice (vk::Device) extensions.
     */
    template<>
    struct context::extension_properties_trait<vk::Device>
    {
        using type = std::vector<vk::ExtensionProperties>;
    };

    /*!
     * @name constext::extension_properties_trait
     * @brief Specialization of the struct context::extension_properties_trait
     * for the VkDebugReportCallbackEXT (vk::DebugReportCallbackEXT) extensions.
     */
    template<>
    struct context::extension_properties_trait<vk::DebugReportCallbackEXT>
    {
        using type = std::vector<vk::LayerProperties>;
    };
}

#endif //MARSUPIAL_VULKAN_CONTEXT_HPP
