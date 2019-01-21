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


#ifndef ENGINE_VERTEX_BUFFER_HPP
#define ENGINE_VERTEX_BUFFER_HPP

#include "memory_allocator.hpp"
#include "../graphics/vertex.hpp"
#include "../twe_core.hpp"

namespace twe::vulkan
{
    class vertex_buffer
    {
    public:
        struct create_info_type
        {
            create_info_type(
                const VmaAllocator memory_allocator = VK_NULL_HANDLE,
                const vk::Queue transfer_queue = vk::Queue( ),
                const vk::CommandBuffer transfer_command_buffer = vk::CommandBuffer( ),
                const uint32_t queue_family_index_count = 0,
                uint32_t* p_queue_family_indices = nullptr,
                const uint32_t vertex_count = 0,
                vertex* p_vertices = nullptr )
                :
                memory_allocator_( memory_allocator ),
                transfer_queue_( transfer_queue ),
                transfer_command_buffer_( transfer_command_buffer ),
                queue_family_index_count_( queue_family_index_count ),
                p_queue_family_indices_( p_queue_family_indices ),
                vertex_count_( vertex_count ),
                p_vertices_( p_vertices )
            { }
            
            create_info_type& set_memory_allocator( const VmaAllocator memory_allocator )
            {
                memory_allocator_ = memory_allocator;
                return *this;
            }
            create_info_type& set_transfer_queue( const vk::Queue transfer_queue )
            {
                transfer_queue_ = transfer_queue;
                return *this;
            }
            create_info_type& set_transfer_command_buffer( const vk::CommandBuffer transfer_command_buffer )
            {
                transfer_command_buffer_ = transfer_command_buffer;
                return *this;
            }
            create_info_type& set_queue_family_index_count_( const uint32_t queue_family_index_count )
            {
                queue_family_index_count_ = queue_family_index_count;
                return *this;
            }
            create_info_type& set_p_queue_family_indices( uint32_t* p_queue_family_indices )
            {
                p_queue_family_indices_ = p_queue_family_indices;
                return *this;
            }
            create_info_type& set_vertex_count( const uint32_t vertex_count )
            {
                vertex_count_ = vertex_count;
                return *this;
            }
            create_info_type& set_p_vertices( vertex* p_vertices )
            {
                p_vertices_ = p_vertices;
                return *this;
            }
            
            
            VmaAllocator memory_allocator_;
            
            vk::Queue transfer_queue_;
            vk::CommandBuffer transfer_command_buffer_;
            
            uint32_t queue_family_index_count_;
            uint32_t* p_queue_family_indices_;
            
            uint32_t vertex_count_;
            vertex* p_vertices_;
        };
        
    public:
        TWE_API vertex_buffer( ) = default;
        TWE_API vertex_buffer( const create_info_type& create_info );
        TWE_API vertex_buffer( const vertex_buffer& rhs ) = delete;
        TWE_API vertex_buffer( vertex_buffer&& rhs ) noexcept;
        TWE_API ~vertex_buffer( );
        
        TWE_API vertex_buffer& operator=( const vertex_buffer& rhs ) = delete;
        TWE_API vertex_buffer& operator=( vertex_buffer&& rhs ) noexcept;
        
        TWE_API const vk::Buffer& get( ) const noexcept;
        
    private:
        VmaAllocator memory_allocator_;
        VmaAllocation memory_allocation_;
        
        vk::Buffer buffer_;
    };
}

#endif //ENGINE_VERTEX_BUFFER_HPP
