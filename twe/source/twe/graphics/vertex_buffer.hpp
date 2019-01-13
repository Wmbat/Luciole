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

#include <vma/vk_mem_alloc.h>

#include "../vulkan/vk_memory_allocator.hpp"
#include "../twe_core.hpp"
#include "vertex.hpp"


namespace twe
{
    class vertex_buffer
    {
    public:
        TWE_API vertex_buffer( ) = default;
        TWE_API vertex_buffer( vk_memory_allocator& memory_allocator, const std::vector<vertex>& vertices );
        TWE_API vertex_buffer( const vertex_buffer& rhs ) noexcept = delete;
        TWE_API vertex_buffer( vertex_buffer&& rhs ) noexcept;
        TWE_API ~vertex_buffer( );
        
        TWE_API vertex_buffer& operator=( const vertex_buffer& rhs ) noexcept = delete;
        TWE_API vertex_buffer& operator=( vertex_buffer&& rhs ) noexcept;
        
        TWE_API const vk::Buffer& get( ) const noexcept;
        
    private:
        VmaAllocator* p_memory_allocator_;
        VmaAllocation memory_allocation_;
        
        vk::Buffer buffer_;
    };
}

#endif //ENGINE_VERTEX_BUFFER_HPP
