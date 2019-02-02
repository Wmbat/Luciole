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


#ifndef MARSUPIAL_VULKAN_BUFFER_HPP
#define MARSUPIAL_VULKAN_BUFFER_HPP

#include "memory_allocator.hpp"
#include "../marsupial_core.hpp"
#include "../graphics/vertex.hpp"
#include "../graphics/mesh.hpp"

namespace marsupial::vulkan
{
    enum class buffer_type
    {
        vertex,
        index
    };

    struct vertex_buffer_create_info_t
    {
        vertex_buffer_create_info_t(
            const VmaAllocator memory_allocator = VK_NULL_HANDLE,
            const vk::Queue tranfer_queue = vk::Queue( ),
            const vk::CommandBuffer transfer_command_buffer = vk::CommandBuffer( ),
            const uint32_t queue_family_index_count = 0,
            uint32_t* p_queue_family_indices = nullptr,
            const uint32_t vertex_count = 0,
            vertex* p_vertices = nullptr )
            :
            memory_allocator_( memory_allocator ),
            transfer_queue_( tranfer_queue ),
            transfer_command_buffer_( transfer_command_buffer ),
            queue_family_index_count_( queue_family_index_count ),
            p_queue_family_indices_( p_queue_family_indices ),
            vertex_count_( vertex_count ),
            p_vertices_( p_vertices )
        {
        
        }
        
        vertex_buffer_create_info_t& set_memory_allocator( const VmaAllocator memory_allocator )
        {
            memory_allocator_ = memory_allocator;
            return *this;
        }
        vertex_buffer_create_info_t& set_transfer_queue( const vk::Queue transfer_queue )
        {
            transfer_queue_ = transfer_queue;
            return *this;
        }
        vertex_buffer_create_info_t& set_transfer_command_buffer( const vk::CommandBuffer transfer_command_buffer )
        {
            transfer_command_buffer_ = transfer_command_buffer;
            return *this;
        }
        vertex_buffer_create_info_t& set_queue_family_index_count( const uint32_t queue_family_index_count )
        {
            queue_family_index_count_ = queue_family_index_count;
            return *this;
        }
        vertex_buffer_create_info_t& set_p_queue_family_indices( uint32_t* p_queue_family_indices )
        {
            p_queue_family_indices_ = p_queue_family_indices;
            return *this;
        }
        vertex_buffer_create_info_t& set_vertex_count( const uint32_t vertex_count )
        {
            vertex_count_ = vertex_count;
            return *this;
        }
        vertex_buffer_create_info_t& set_p_vertices( vertex* p_vertices )
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
    
    struct index_buffer_create_info_t
    {
        index_buffer_create_info_t(
            const VmaAllocator memory_allocator = VK_NULL_HANDLE,
            const vk::Queue tranfer_queue = vk::Queue( ),
            const vk::CommandBuffer transfer_command_buffer = vk::CommandBuffer( ),
            const uint32_t queue_family_index_count = 0,
            uint32_t* p_queue_family_indices = nullptr,
            const uint32_t index_count = 0,
            uint32_t* p_indices = nullptr )
            :
            memory_allocator_( memory_allocator ),
            transfer_queue_( tranfer_queue ),
            transfer_command_buffer_( transfer_command_buffer ),
            queue_family_index_count_( queue_family_index_count ),
            p_queue_family_indices_( p_queue_family_indices ),
            index_count_( index_count ),
            p_indices_( p_indices )
        {
        
        }
    
        index_buffer_create_info_t& set_memory_allocator( const VmaAllocator memory_allocator )
        {
            memory_allocator_ = memory_allocator;
            return *this;
        }
        index_buffer_create_info_t& set_transfer_queue( const vk::Queue transfer_queue )
        {
            transfer_queue_ = transfer_queue;
            return *this;
        }
        index_buffer_create_info_t& set_transfer_command_buffer( const vk::CommandBuffer transfer_command_buffer )
        {
            transfer_command_buffer_ = transfer_command_buffer;
            return *this;
        }
        index_buffer_create_info_t& set_queue_family_index_count( const uint32_t queue_family_index_count )
        {
            queue_family_index_count_ = queue_family_index_count;
            return *this;
        }
        index_buffer_create_info_t& set_p_queue_family_indices( uint32_t* p_queue_family_indices )
        {
            p_queue_family_indices_ = p_queue_family_indices;
            return *this;
        }
        index_buffer_create_info_t& set_vertex_count( const uint32_t index_count )
        {
            index_count_ = index_count;
            return *this;
        }
        index_buffer_create_info_t& set_p_vertices( uint32_t* p_indices )
        {
            p_indices_ = p_indices;
            return *this;
        }
        
        VmaAllocator memory_allocator_;
    
        vk::Queue transfer_queue_;
        vk::CommandBuffer transfer_command_buffer_;
    
        uint32_t queue_family_index_count_;
        uint32_t* p_queue_family_indices_;
    
        uint32_t index_count_;
        uint32_t* p_indices_;
    };

    // TODO: Handle only 1 queue.
    template<buffer_type type>
    class buffer
    {
    public:
        buffer( ) = default;
        buffer( const vertex_buffer_create_info_t& create_info )
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
        buffer( const index_buffer_create_info_t& create_info )
            :
            memory_allocator_( create_info.memory_allocator_ )
        {
            const auto buffer_size = create_info.index_count_ * sizeof( create_info.p_indices_[0] );
    
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
            memcpy( data, create_info.p_indices_, buffer_size );
            vmaUnmapMemory( memory_allocator_, staging_memory );
    
            // Vertex Buffer //
            const auto vertex_create_info = vk::BufferCreateInfo( )
                .setSize( buffer_size )
                .setUsage( vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer )
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
        buffer( const buffer& rhs ) = delete;
        buffer( buffer&& rhs ) noexcept
        {
            *this = std::move( rhs );
        }
        ~buffer( )
        {
            if( memory_allocator_ != VK_NULL_HANDLE )
                vmaDestroyBuffer( memory_allocator_, buffer_, memory_allocation_ );
        }
        
        buffer& operator=( const buffer& rhs ) = delete;
        buffer& operator=( buffer&& rhs ) noexcept
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
        
        const vk::Buffer& get( ) const noexcept
        {
            return buffer_;
        }
        
    private:
        VmaAllocator memory_allocator_;
        VmaAllocation memory_allocation_;
    
        vk::Buffer buffer_;
    };
}

#endif //MARSUPIAL_VULKAN_BUFFER_HPP
