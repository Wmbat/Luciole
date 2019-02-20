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

#ifndef MARSUPIAL_VULKAN_MESH_BUFFER_HPP
#define MARSUPIAL_VULKAN_MESH_BUFFER_HPP

#include "vulkan.hpp"

#include "vma/vk_mem_alloc.h"

#include "../graphics/mesh.hpp"

namespace marsupial::vulkan
{
	enum class mesh_buffer_attribute
	{
		position,
		colour,
		index
	};

	struct mesh_buffer_create_info
	{
		mesh_buffer_create_info(
			const VmaAllocator memory_allocator = VK_NULL_HANDLE,
			const vk::Queue transfer_queue = vk::Queue{ },
			const vk::CommandBuffer transfer_command_buffer = vk::CommandBuffer{ },
			const uint32_t queue_family_index_count = 0,
			uint32_t* p_queue_family_indices = nullptr,
			const mesh data = mesh{ } )
			:
			memory_allocator_( memory_allocator ),
			transfer_queue_( transfer_queue ),
			transfer_command_buffer_( transfer_command_buffer ),
			queue_family_index_count_( queue_family_index_count ),
			p_queue_family_indices_( p_queue_family_indices ),
			data_( data )
		{	}

		mesh_buffer_create_info& set_memory_allocator( const VmaAllocator memory_allocator )
		{
			memory_allocator_ = memory_allocator;
			return *this;
		}
		mesh_buffer_create_info& set_transfer_queue( const vk::Queue transfer_queue )
		{
			transfer_queue_ = transfer_queue;
			return *this;
		}
		mesh_buffer_create_info& set_transfer_command_buffer( const vk::CommandBuffer transfer_command_buffer )
		{
			transfer_command_buffer_ = transfer_command_buffer;
			return *this;
		}
		mesh_buffer_create_info& set_queue_family_index_count( const uint32_t queue_family_index_count )
		{
			queue_family_index_count_ = queue_family_index_count;
			return *this;
		}
		mesh_buffer_create_info& set_p_queue_family_indices( uint32_t* p_queue_family_indices )
		{
			p_queue_family_indices_ = p_queue_family_indices;
			return *this;
		}
		mesh_buffer_create_info& set_data( const mesh data )
		{
			data_ = data;
			return *this;
		}

		VmaAllocator memory_allocator_;

		vk::Queue transfer_queue_;
		vk::CommandBuffer transfer_command_buffer_;

		uint32_t queue_family_index_count_;
		uint32_t* p_queue_family_indices_;

		mesh data_;
	};

	class mesh_buffer
	{
	public:
		mesh_buffer( ) = default;
		mesh_buffer( const mesh_buffer_create_info& create_info );
		mesh_buffer( const mesh_buffer& rhs ) = delete;
		mesh_buffer( mesh_buffer&& rhs ) noexcept;
		~mesh_buffer( );

		mesh_buffer& operator=( const mesh_buffer& rhs ) = delete;
		mesh_buffer& operator=( mesh_buffer&& rhs ) noexcept;

		const vk::Buffer& get( ) const noexcept;
		vk::Buffer& get( ) noexcept;
		
		template<mesh_buffer_attribute T>
		std::enable_if_t<T == mesh_buffer_attribute::position, const vk::DeviceSize> get_offset( )
		{
			return positions_offset_;
		}
		template<mesh_buffer_attribute T>
		std::enable_if_t<T == mesh_buffer_attribute::colour, const vk::DeviceSize> get_offset( )
		{
			return colours_offset_;
		}
		template<mesh_buffer_attribute T>
		std::enable_if_t<T == mesh_buffer_attribute::index, const vk::DeviceSize> get_offset( )
		{
			return indices_offset_;
		}

	private:
		VmaAllocator memory_allocator_;
		VmaAllocation memory_allocation_;

		vk::Buffer buffer_;

		uint32_t positions_offset_;
		uint32_t colours_offset_;
		uint32_t indices_offset_;
	};
}

#endif // MARSUPIAL_VULKAN_MESH_BUFFER_HPP