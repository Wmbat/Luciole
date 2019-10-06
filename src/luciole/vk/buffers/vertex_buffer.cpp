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

#include <luciole/vk/buffers/vertex_buffer.hpp>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace vk
{
    vertex_buffer::vertex_buffer( vertex_buffer::create_info_cref_t create_info )
        :
        memory_allocator_( create_info.value_.memory_allocator )
    {
        auto const buffer_size = create_info.value_.vertices_.size() * sizeof( create_info.value_.vertices_[0] );

        VkBuffer staging_buffer = VK_NULL_HANDLE;
        VmaAllocation staging_memory = VK_NULL_HANDLE; 

        /* STAGING BUFFER */
        VkBufferCreateInfo const staging_buffer_create_info 
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = buffer_size,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .sharingMode = VK_SHARING_MODE_CONCURRENT,
            .queueFamilyIndexCount = static_cast<std::uint32_t>( create_info.value_.family_indices.size() ),
            .pQueueFamilyIndices = create_info.value_.family_indices.data()
        };

        VmaAllocationCreateInfo staging_allocation_info = { };
        staging_allocation_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;  // TODO: remove hardcode query for it

        vmaCreateBuffer( memory_allocator_,
            reinterpret_cast<const VkBufferCreateInfo*>( &staging_buffer_create_info ), &staging_allocation_info,
            reinterpret_cast<VkBuffer*>( &staging_buffer ), &staging_memory, nullptr 
        );
        ///////////////////
    }
} // namespace vk
