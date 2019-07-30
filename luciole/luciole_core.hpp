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


#ifndef LUCIOLE_LUCIOLE_CORE_HPP
#define LUCIOLE_LUCIOLE_CORE_HPP

#include <cstdint>
#include <optional>
#include <string>

#include "utilities/log.hpp"

#include "strong_types.hpp"

struct error_msg_parameter { };
using error_msg_t = strong_type<std::string, error_msg_parameter>;

template<typename type>
type vk_check( const type handle, const error_msg_t& err_msg )
{
    if ( handle == VK_NULL_HANDLE )
    {
        core_error( err_msg.value_ );
        throw;
    }

    return handle;
}

template<typename type>
std::vector<type> vk_check_array( const std::vector<type>& handles, const error_msg_t& err_msg )
{
    if ( handles.size( ) == 0 )
    {
        core_error( err_msg.value_ );
        throw;
    }

    return handles;
}

static constexpr uint32_t kilobyte = 1024;
static constexpr uint32_t megabyte = kilobyte * kilobyte;
    
constexpr unsigned long long operator "" _kg( unsigned long long size ) { return size * kilobyte; }
constexpr unsigned long long operator "" _mb( unsigned long long size ) { return size * megabyte; }

#endif //LUCIOLE_LUCIOLE_CORE_HPP