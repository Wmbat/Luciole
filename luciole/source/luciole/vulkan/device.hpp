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


#ifndef LUCIOLE_VULKAN_DEVICE_HPP
#define LUCIOLE_VULKAN_DEVICE_HPP

#include <vector>

#include "utils.hpp"

#include "command_pool.hpp"
#include "instance.hpp"
#include "surface.hpp"
#include "queue.hpp"

namespace lcl::vulkan
{
    struct device
    {
    private:
        struct queue_family_info;

    public:
        LUCIOLE_API device( ) = default;
        LUCIOLE_API device( const instance& ins1tance, const surface& surface );
        device( const device& other ) = delete;
        LUCIOLE_API device( device&& other );
        LUCIOLE_API ~device( );

        device& operator=( const device& rhs ) = delete;
        LUCIOLE_API device& operator=( device&& rhs );

        LUCIOLE_API std::vector<command_pool> get_command_pools( ) const;

    private:
        /**
         * @brief - Check if a graphics card is suitable for rendering
         * 
         * @param [in] gpu - The graphics card to check for rendering support.
         * @param [in] surface - The Surface to use for rendering.
         * @return - true if the graphics card is suitable for rendering.
         * @return - false if the graphics card does not meet the requirements for rendering.
         */
        bool is_gpu_suitable( VkPhysicalDevice gpu, const surface& surface ) const;

        /**
         * @brief - Get the queue family infos object
         * 
         * @param - gpu 
         * @return - std::vector<queue_family_info> 
         */
        std::vector<queue_family_info> get_queue_family_infos( VkPhysicalDevice gpu ) const;

    public:
        VkPhysicalDevice gpu_;
        VkDevice handle_ = VK_NULL_HANDLE;

        std::vector<queue> queues_;

    private:
        VkInstance instance_;
        VkSurfaceKHR surface_;

        device_extensions extensions_;

    private:
        struct queue_family_info
        {
            queue_family_info(
                const std::uint32_t queue_count = 0,
                const std::optional<std::uint32_t> graphics = std::nullopt,
                const std::optional<std::uint32_t> present = std::nullopt,
                const std::optional<std::uint32_t> transfer = std::nullopt );
        
            queue_family_info& set_queue_count( const std::uint32_t queue_count ) noexcept;
            queue_family_info& set_graphics( const std::uint32_t index ) noexcept;
            queue_family_info& set_present( const std::uint32_t index ) noexcept;
            queue_family_info& set_transfer( const std::uint32_t index ) noexcept;

            /**
             * @brief - Helper func to check what the queue is capable of rendering.
             * 
             * @return - true if the queue is capable of rendering.
             * @return - false if the queue is not capable of rendering.
             */
            bool has_rendering_support( ) const noexcept;

            /**
             * @brief - Helper func to check if the queue is capable of everything (rendering & transfer).
             * 
             * @return - true if the queue is has a general purpose (rendering & transfer )
             * @return - false if the queue doesn't have a general purpose (rendering & transfer) 
             */
            bool is_general_purpose( ) const noexcept;

            /**
             * @brief - Checks if the queue is dedicated to transfer commands
             * 
             * @return - true if the queue can only be used for transfer commands.
             * @return - false if the queue purpose is not limited to transfer commands.
             */
            bool is_dedicated_transfer( ) const noexcept;

            /**
             * @brief - Check if the queue is dedicated to graphics commands.
             * 
             * @return true if the queue can only be used for graphics commands.
             * @return false if the queue's purpose is not limited to graphics commands.
             */
            bool is_dedicated_graphics( ) const noexcept;

            std::uint32_t queue_count_ = 0;

            std::optional<std::uint32_t> graphics_ = std::nullopt;
            std::optional<std::uint32_t> present_ = std::nullopt;
            std::optional<std::uint32_t> transfer_ = std::nullopt;
        };
    };
}

#endif // LUCIOLE_VULKAN_DEVICE_HPP
