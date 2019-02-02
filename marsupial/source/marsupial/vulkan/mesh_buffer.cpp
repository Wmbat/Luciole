
#include "mesh_buffer.hpp"

namespace marsupial::vulkan
{
	mesh_buffer::mesh_buffer( const mesh_buffer_create_info_t& create_info )
		:
		memory_allocator_( create_info.memory_allocator_ )
	{
		const auto vertices_size = sizeof( create_info.data_.vertices_[0] ) * create_info.data_.vertices_.size( );
		const auto indices_size = sizeof( create_info.data_.indices_[0] ) * create_info.data_.indices_.size( );
		const auto buffer_size = vertices_size + indices_size;

		vertices_offset_ = 0;
		indices_offset_ = vertices_size;

		auto staging_buffer = vk::Buffer{ };
		auto staging_memory = VmaAllocation{ };

		auto sharing_mode = vk::SharingMode{ };

		if ( create_info.queue_family_index_count_ == 1 )
		{
			sharing_mode = vk::SharingMode::eExclusive;
		}
		else if ( create_info.queue_family_index_count_ > 1 )
		{
			if ( create_info.p_queue_family_indices_[0] == create_info.p_queue_family_indices_[1] )
			{
				sharing_mode = vk::SharingMode::eExclusive;
			}
			else
			{
				sharing_mode = vk::SharingMode::eConcurrent;
			}
		}
		
		const auto staging_create_info = vk::BufferCreateInfo{ }
			.setSize( buffer_size )
			.setUsage( vk::BufferUsageFlagBits::eTransferSrc )
			.setSharingMode( sharing_mode )
			.setQueueFamilyIndexCount( create_info.queue_family_index_count_ )
			.setPQueueFamilyIndices( create_info.p_queue_family_indices_ );

		VmaAllocationCreateInfo staging_allocation_info = { };
		staging_allocation_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;  // TODO: remove hardcode query for it

		vmaCreateBuffer( memory_allocator_,
			reinterpret_cast< const VkBufferCreateInfo* >( &staging_create_info ), &staging_allocation_info,
			reinterpret_cast< VkBuffer* >( &staging_buffer ), &staging_memory, nullptr );

		void* data;
		vmaMapMemory( memory_allocator_, staging_memory, &data );
		memcpy( data, &create_info.data_, buffer_size );
		vmaUnmapMemory( memory_allocator_, staging_memory );

		const auto vertex_create_info = vk::BufferCreateInfo( )
			.setSize( buffer_size )
			.setUsage( vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer )
			.setSharingMode( sharing_mode )
			.setQueueFamilyIndexCount( create_info.queue_family_index_count_ )
			.setPQueueFamilyIndices( create_info.p_queue_family_indices_ );

		VmaAllocationCreateInfo allocation_info = { };
		allocation_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		vmaCreateBuffer( memory_allocator_,
			reinterpret_cast< const VkBufferCreateInfo* >( &vertex_create_info ), &allocation_info,
			reinterpret_cast< VkBuffer* >( &buffer_ ), &memory_allocation_, nullptr );
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
	mesh_buffer::mesh_buffer( mesh_buffer && rhs ) noexcept
	{
		*this = std::move( rhs );
	}
	mesh_buffer::~mesh_buffer( )
	{
		if ( memory_allocator_ != VK_NULL_HANDLE )
			vmaDestroyBuffer( memory_allocator_, buffer_, memory_allocation_ );
	}
	mesh_buffer & mesh_buffer::operator=( mesh_buffer && rhs ) noexcept
	{
		if ( this != &rhs )
		{
			memory_allocator_ = rhs.memory_allocator_;
			rhs.memory_allocator_ = VK_NULL_HANDLE;

			memory_allocation_ = rhs.memory_allocation_;
			rhs.memory_allocation_ = VK_NULL_HANDLE;

			buffer_ = std::move( rhs.buffer_ );
		}

		return *this;
	}
	const vk::Buffer mesh_buffer::get( ) const noexcept
	{
		return buffer_;
	}
}