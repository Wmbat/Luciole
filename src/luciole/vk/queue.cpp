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

#include <luciole/vk/queue.hpp>

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

vk::error::type queue::submit( vk::submit_info_t info, vk::fence_t fence ) const noexcept
{
    switch( vkQueueSubmit( handle_, 1, &info.value_, fence.value_ ) )
    {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
        case VK_ERROR_DEVICE_LOST:
            return vk::error::type::e_device_lost;
        default:
            return vk::error::type::e_none;
    }
}

vk::error::type queue::present( vk::present_info_t info ) const noexcept
{
    switch( vkQueuePresentKHR( handle_, &info.value_ ) )
    {
        case VK_SUBOPTIMAL_KHR:
            return vk::error::type::e_suboptimal;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return vk::error::type::e_out_of_host_memory;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return vk::error::type::e_out_of_device_memory;
        case VK_ERROR_DEVICE_LOST:
            return vk::error::type::e_device_lost;
        case VK_ERROR_OUT_OF_DATE_KHR:
            return vk::error::type::e_out_of_date;
        case VK_ERROR_SURFACE_LOST_KHR:
            return vk::error::type::e_surface_lost;
        default:
            return vk::error::type::e_none;
    }
}

std::uint32_t queue::get_family_index( ) const noexcept
{
    return family_index_;
}