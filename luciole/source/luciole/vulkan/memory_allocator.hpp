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

#ifndef LUCIOLE_VULKAN_MEMORY_ALLOCATOR_HPP
#define LUCIOLE_VULKAN_MEMORY_ALLOCATOR_HPP


#include "vulkan.hpp"
#include "vma/vk_mem_alloc.h"

#include "../luciole_core.hpp"

namespace lcl::vulkan
{
    class memory_allocator
    {
    public:
        LUCIOLE_API memory_allocator( ) = default;
        LUCIOLE_API memory_allocator( const VmaAllocatorCreateInfo& create_info );
        LUCIOLE_API memory_allocator( const memory_allocator& rhs ) = delete;
        LUCIOLE_API memory_allocator( memory_allocator&& rhs ) noexcept;
        LUCIOLE_API ~memory_allocator( );
        
        LUCIOLE_API memory_allocator& operator=( const memory_allocator& rhs ) = delete;
        LUCIOLE_API memory_allocator& operator=( memory_allocator&& rhs ) noexcept;
        
        LUCIOLE_API const VmaAllocator& operator( )( ) const noexcept;
        LUCIOLE_API VmaAllocator& operator( )( ) noexcept;
        
        LUCIOLE_API const VmaAllocator& get( ) const noexcept;
        LUCIOLE_API VmaAllocator& get( ) noexcept;
    
    private:
        VmaAllocator allocator_;
    };
}

#endif //LUCIOLE_VULKAN_MEMORY_ALLOCATOR_HPP
