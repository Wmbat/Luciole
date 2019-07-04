/*
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

#include <vector>

#include <vulkan/vulkan.h>

#include "luciole_core.hpp"

namespace lcl::core
{
#if defined( NDEBUG )
    static constexpr bool enable_debug_layers = false;
#else
    static constexpr bool enable_debug_layers = true;
#endif

    class context
    {
    public:
        LUCIOLE_API context( ) = default;
        LUCIOLE_API virtual ~context( ) = default;

    protected:
        LUCIOLE_API void create_instance( const VkApplicationInfo& app_info ) noexcept;
        LUCIOLE_API void destroy_instance( ) noexcept;
        
        LUCIOLE_API void create_debug_messenger( ) noexcept;
        LUCIOLE_API void destroy_debug_messenger( ) noexcept;

        LUCIOLE_API void create_device( ) noexcept;
        LUCIOLE_API void destroy_device( ) noexcept;

        virtual void pick_gpu( ) noexcept { };
        virtual int rate_gpu( const VkPhysicalDevice gpu ) noexcept { };

    protected:
        struct queue_info
        {
            bool is_compute_only( ) const noexcept
            {
                return flags_ == VK_QUEUE_COMPUTE_BIT;
            }
            bool is_transfer_only( ) const noexcept
            {
                return flags_ == VK_QUEUE_TRANSFER_BIT;
            }
            bool is_graphics_only( ) const noexcept
            {
                return flags_ == VK_QUEUE_GRAPHICS_BIT;
            }
            bool is_general_purpose( ) const noexcept
            {
                return ( flags_ & VK_QUEUE_COMPUTE_BIT ) && ( flags_ & VK_QUEUE_TRANSFER_BIT ) && ( flags_ & VK_QUEUE_GRAPHICS_BIT );
            }

            VkQueueFlags flags_ = { };
            std::uint32_t count_ = 0;
            std::uint32_t index_ = 0;
        };

        struct queue
        {
            bool is_compute_only( )
            {
                return flags_ == VK_QUEUE_COMPUTE_BIT;
            }
            bool is_transfer_only( )
            {
                return flags_ == VK_QUEUE_TRANSFER_BIT;
            }
            bool is_graphics_only( )
            {
                return flags_ == VK_QUEUE_GRAPHICS_BIT;
            }
            bool is_general_purpose( )
            {
                return ( flags_ & VK_QUEUE_COMPUTE_BIT ) && ( flags_ & VK_QUEUE_TRANSFER_BIT ) && ( flags_ & VK_QUEUE_GRAPHICS_BIT );
            }

            VkQueueFlags flags_ = { };
            VkQueue handle_ = VK_NULL_HANDLE;
            std::uint32_t index_ = 0;
            std::uint32_t family_ = 0;
        };

    protected:
        VkInstance instance_ = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
        VkPhysicalDevice gpu_ = VK_NULL_HANDLE;
        VkDevice device_ = VK_NULL_HANDLE;

        std::vector<queue> queues_;

        std::vector<const char*> instance_extensions;
        std::vector<const char*> device_extensions_;

        const std::vector<const char*> validation_layers = {
            "VK_LAYER_KHRONOS_validation"
        };
    };
} // namespace lcl