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

namespace twe::vulkan
{
    vertex_buffer::vertex_buffer( const create_info_type& create_info  )
        :
        memory_allocator_( create_info.memory_allocator_ )
    {
        const auto buffer_size = create_info.vertex_count_ * sizeof( create_info.p_vertices_[0] );
        
        auto staging_buffer = vk::Buffer( );
        auto staging_memory = VmaAllocation( );
        
        // Staging Buffer //
        const auto staging_create_info = vk::BufferCreateInfo( )
            .setSize( buffer_size )
            .setUsage( vk::BufferUsageFlagBits::eTransferSrc )
            .setSharingMode( vk::SharingMode::eConcurrent )
            .setQueueFamilyIndexCount( create_info.queue_family_index_count_ )
            .setPQueueFamilyIndices( create_info.p_queue_family_indices_ );
        
        VmaAllocationCreateInfo staging_allocation_info = { };
        staging_allocation_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;  // TODO: remove hardcode query for it
        
        vmaCreateBuffer( memory_allocator_,
            reinterpret_cast<const VkBufferCreateInfo*>( &staging_create_info ), &staging_allocation_info,
            reinterpret_cast<VkBuffer*>( &staging_buffer ), &staging_memory, nullptr );
        ///////////////////
        
        void* data;
        vmaMapMemory( memory_allocator_, staging_memory, &data );
        memcpy( data, create_info.p_vertices_, buffer_size );
        vmaUnmapMemory( memory_allocator_, staging_memory );
        
        // Vertex Buffer //
        const auto vertex_create_info = vk::BufferCreateInfo( )
            .setSize( buffer_size )
            .setUsage( vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer )
            .setSharingMode( vk::SharingMode::eConcurrent )
            .setQueueFamilyIndexCount( create_info.queue_family_index_count_ )
            .setPQueueFamilyIndices( create_info.p_queue_family_indices_ );
        
        VmaAllocationCreateInfo allocation_info = { };
        allocation_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        
        vmaCreateBuffer( memory_allocator_,
            reinterpret_cast<const VkBufferCreateInfo*>( &vertex_create_info ), &allocation_info,
            reinterpret_cast<VkBuffer*>( &buffer_ ), &memory_allocation_, nullptr );
        ///////////////////
        
        
        const auto begin_info = vk::CommandBufferBeginInfo( )
            .setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );
        
        create_info.transfer_command_buffer_.begin( begin_info );
        
        const auto copy_region = vk::BufferCopy( )
            .setSrcOffset( 0 )
            .setDstOffset( 0 )
            .setSize( buffer_size );
    
        create_info.transfer_command_buffer_.copyBuffer( staging_buffer, buffer_, copy_region );
    
        create_info.transfer_command_buffer_.end( );
        
        const auto submit_info = vk::SubmitInfo( )
            .setCommandBufferCount( 1 )
            .setPCommandBuffers( &create_info.transfer_command_buffer_ );
        
        // TODO: Use Fence.
        create_info.transfer_queue_.submit( submit_info, nullptr );
        create_info.transfer_queue_.waitIdle( );
        
        vmaDestroyBuffer( memory_allocator_, staging_buffer, staging_memory );
    }
    vertex_buffer::vertex_buffer( vertex_buffer&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    vertex_buffer::~vertex_buffer( )
    {
        if( memory_allocator_ != VK_NULL_HANDLE )
            vmaDestroyBuffer( memory_allocator_, buffer_, memory_allocation_ );
    }
    
    vertex_buffer& vertex_buffer::operator=( vertex_buffer&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            memory_allocator_ = rhs.memory_allocator_;
            rhs.memory_allocator_ = VK_NULL_HANDLE;
            
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
