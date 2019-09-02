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

#include "queue.hpp"

queue::queue( vk::device_t device, family_index_t family_index, index_t index )
    :
    family_index_( family_index.value_ ),
    index_( index.value_ )
{
    vkGetDeviceQueue( device.value_, family_index_, index_, &handle_ );
}
queue::queue( queue&& rhs )
{
    *this = std::move( rhs );
}

queue& queue::operator=( queue&& rhs )
{
    if ( this != &rhs )
    {
        std::swap( handle_, rhs.handle_ );
        std::swap( family_index_, rhs.family_index_ );
        std::swap( index_, rhs.index_ );
    }

    return *this;
}

bool queue::submit( vk::submit_info_t info, vk::fence_t fence ) const noexcept
{
    return ( vkQueueSubmit( handle_, 1, &info.value_, fence.value_ ) == VK_SUCCESS ) ? true : false;
}

void queue::present( vk::present_info_t info ) const noexcept
{
    vkQueuePresentKHR( handle_, &info.value_ );   
}

std::uint32_t queue::get_family_index( ) const noexcept
{
    return family_index_;
}