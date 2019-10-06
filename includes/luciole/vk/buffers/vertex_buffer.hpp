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

#ifndef LUCIOLE_VK_BUFFERS_VERTEX_BUFFER_HPP
#define LUCIOLE_VK_BUFFERS_VERTEX_BUFFER_HPP

#include <luciole/strong_types.hpp>
#include <luciole/graphics/vertex.hpp>
#include <luciole/vk/queue.hpp>

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace vk
{
    class vertex_buffer
    {
    public:
        struct create_info;
        using create_info_cref_t = strong_type<create_info const&>;

    public:
        vertex_buffer( ) = default;
        vertex_buffer( create_info_cref_t create_info );

    private:
        VmaAllocator memory_allocator_;
        VmaAllocation memory_allocation_;

        VkBuffer buffer_;

    public:
        struct create_info
        {
            VmaAllocator memory_allocator = VK_NULL_HANDLE;
            VkCommandBuffer command_buffer = VK_NULL_HANDLE;
            
            queue* p_queue;
            
            std::vector<std::uint32_t> family_indices; 
            std::vector<vertex> vertices_;
        };
    };
} // namespace vk



#endif // LUCIOLE_VK_BUFFERS_VERTEX_BUFFER_HPP