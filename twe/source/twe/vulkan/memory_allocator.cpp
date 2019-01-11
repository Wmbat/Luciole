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

#include "memory_allocator.hpp"
#include "../utilities/basic_error.hpp"
#include "../utilities/log.hpp"

namespace twe
{
    memory_allocator::memory_allocator( const VmaAllocatorCreateInfo& create_info )
    {
        try
        {
            auto result = vmaCreateAllocator( &create_info, &allocator_ );
    
            if( result != VK_SUCCESS )
            {
                throw basic_error{ basic_error::code::engine_error, "Failed to create VMA allocator." };
            }
        }
        catch( const basic_error& e )
        {
            core_error( e.what() );
        }
    
    }
    memory_allocator::memory_allocator( memory_allocator&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    memory_allocator::~memory_allocator( )
    {
        vmaDestroyAllocator( allocator_ );
    }
    
    memory_allocator& memory_allocator::operator=( memory_allocator&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            allocator_ = rhs.allocator_;
            rhs.allocator_ = VK_NULL_HANDLE;
        }
        
        return *this;
    }
    
    const VmaAllocator& memory_allocator::operator()( ) const noexcept
    {
        return allocator_;
    }
    VmaAllocator& memory_allocator::operator( )( ) noexcept
    {
        return allocator_;
    }
    
    const VmaAllocator& memory_allocator::get( ) const noexcept
    {
        return allocator_;
    }
    VmaAllocator& memory_allocator::get( ) noexcept
    {
        return allocator_;
    }
}

