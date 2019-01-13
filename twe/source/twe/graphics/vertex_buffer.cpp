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

#include "vertex_buffer.hpp"

namespace twe
{
    vertex_buffer::vertex_buffer( vk_memory_allocator& memory_allocator, const std::vector<vertex>& vertices  )
        :
        p_memory_allocator_( reinterpret_cast<VmaAllocator*>( &memory_allocator  ) )
    {
        const auto buffer_size = vertices.size() * sizeof( vertices[0] );
        
        const auto staging_create_info = vk::BufferCreateInfo( )
            .setSize( buffer_size )
            .setUsage( vk::BufferUsageFlagBits::eTransferSrc )
            .setSharingMode( vk::SharingMode::eExclusive );
        
        vk::Buffer staging_buffer;
        VmaAllocation staging_buffer_memory;
        
        VmaAllocationCreateInfo staging_allocation_info = { };
        staging_allocation_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;  // TODO: remove hardcode query for it
        
        vmaCreateBuffer( *p_memory_allocator_,
            reinterpret_cast<const VkBufferCreateInfo*>( &staging_create_info ), &staging_allocation_info,
            reinterpret_cast<VkBuffer*>( &staging_buffer ), &staging_buffer_memory, nullptr );
        
        void* data;
        vmaMapMemory( *p_memory_allocator_, staging_buffer_memory, &data );
        memcpy( data, vertices.data(), buffer_size );
        vmaUnmapMemory( *p_memory_allocator_, staging_buffer_memory );
        
        const auto vertex_create_info = vk::BufferCreateInfo( )
            .setSize( buffer_size )
            .setUsage( vk::BufferUsageFlagBits::eTransferDst )
            .setSharingMode( vk::SharingMode::eExclusive );
        
        VmaAllocationCreateInfo allocation_info = { };
        allocation_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    }
    vertex_buffer::vertex_buffer( vertex_buffer&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    vertex_buffer::~vertex_buffer( )
    {
        if( p_memory_allocator_ != nullptr )
            vmaDestroyBuffer( *p_memory_allocator_, buffer_, memory_allocation_ );
    }
    
    vertex_buffer& vertex_buffer::operator=( vertex_buffer&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            p_memory_allocator_ = rhs.p_memory_allocator_;
            rhs.p_memory_allocator_ = nullptr;
            
            memory_allocation_ = rhs.memory_allocation_;
            rhs.memory_allocation_ = VK_NULL_HANDLE;
            
            buffer_ = std::move( rhs.buffer_ );
        }
        
        return *this;
    }
    
    const vk::Buffer& vertex_buffer::get( ) const noexcept
    {
        return buffer_;
    }
}
