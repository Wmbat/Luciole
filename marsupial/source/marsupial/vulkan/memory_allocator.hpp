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

#ifndef MARSUPIAL_VULKAN_MEMORY_ALLOCATOR_HPP
#define MARSUPIAL_VULKAN_MEMORY_ALLOCATOR_HPP


#include "vulkan.hpp"
#include "vma/vk_mem_alloc.h"
#include "../marsupial_core.hpp"

namespace marsupial::vulkan
{
    class memory_allocator
    {
    public:
        MARSUPIAL_API memory_allocator( ) = default;
        MARSUPIAL_API memory_allocator( const VmaAllocatorCreateInfo& create_info );
        MARSUPIAL_API memory_allocator( const memory_allocator& rhs ) = delete;
        MARSUPIAL_API memory_allocator( memory_allocator&& rhs ) noexcept;
        MARSUPIAL_API ~memory_allocator( );
        
        MARSUPIAL_API memory_allocator& operator=( const memory_allocator& rhs ) = delete;
        MARSUPIAL_API memory_allocator& operator=( memory_allocator&& rhs ) noexcept;
        
        MARSUPIAL_API const VmaAllocator& operator( )( ) const noexcept;
        MARSUPIAL_API VmaAllocator& operator( )( ) noexcept;
        
        MARSUPIAL_API const VmaAllocator& get( ) const noexcept;
        MARSUPIAL_API VmaAllocator& get( ) noexcept;
    
    private:
        VmaAllocator allocator_;
    };
}

#endif //MARSUPIAL_VULKAN_MEMORY_ALLOCATOR_HPP
