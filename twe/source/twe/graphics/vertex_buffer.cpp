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
    vertex_buffer::vertex_buffer( memory_allocator& p_memory_allocator, const std::vector<vertex>& vertices  )
        :
        p_memory_allocator_( reinterpret_cast<VmaAllocator*>( &p_memory_allocator  ) )
    {
        const auto create_info = vk::BufferCreateInfo( )
            .setSize( vertices.size() * sizeof( vertices[0] ) )
            .setUsage( vk::BufferUsageFlagBits::eVertexBuffer )
            .setSharingMode( vk::SharingMode::eExclusive );
    
        
        VmaAllocationCreateInfo allocation_info = { };
        allocation_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;  // TODO: remove hardcode query for it
        
        vmaCreateBuffer( *p_memory_allocator_,
            reinterpret_cast<const VkBufferCreateInfo*>( &create_info ), &allocation_info,
            reinterpret_cast<VkBuffer*>( &buffer_ ), &memory_allocation_, nullptr );
        
        void* data;
        vmaMapMemory( *p_memory_allocator_, memory_allocation_, &data );
        memcpy( data, vertices.data(), vertices.size() );
        vmaUnmapMemory( *p_memory_allocator_, memory_allocation_ );
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
    
    uint32_t find_memory_type( vk::PhysicalDevice& gpu, uint32_t type_filter, vk::MemoryPropertyFlags flags )
    {
        auto mem_properties = gpu.getMemoryProperties( );
        
        for( uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i )
        {
            if ( ( type_filter & ( 1 << i ) ) && ( mem_properties.memoryTypes[i].propertyFlags & flags ) == flags )
            {
                return i;
            }
        }
    }
    
    vertex_buffer::vertex_buffer( vk::Device* p_device, vk::PhysicalDevice& gpu, const std::vector<vertex>& vertices )
        :
        p_device_( p_device )
    {
        const auto create_info = vk::BufferCreateInfo( )
            .setSize( vertices.size() * sizeof( vertices[0] ) )
            .setUsage( vk::BufferUsageFlagBits::eVertexBuffer )
            .setSharingMode( vk::SharingMode::eExclusive );
        
        buffer_ = p_device_->createBuffer( create_info );
        
        auto mem_reqs = p_device_->getBufferMemoryRequirements( buffer_ );
        
        const auto alloc_info = vk::MemoryAllocateInfo( )
            .setAllocationSize( mem_reqs.size )
            .setMemoryTypeIndex( find_memory_type( gpu, mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent ) );
        
        memory_ = p_device_->allocateMemory( alloc_info );
        
        p_device_->bindBufferMemory( buffer_, memory_, 0 );
        
        void* data;
        p_device_->mapMemory( memory_, 0, create_info.size,{ }, &data );
        memcpy( data, vertices.data(), create_info.size );
        p_device_->unmapMemory( memory_ );
    }
}
