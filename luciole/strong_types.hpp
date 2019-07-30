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

#ifndef LUCIOLE_STRONG_TYPES_HPP
#define LUCIOLE_STRONG_TYPES_HPP

#include <cstdint>
#include <string>

template <typename type, typename parameter>
class strong_type
{
public:
    explicit strong_type( type const& value ) 
        : 
        value_( value ) 
    {   }

    template<typename type_ = type>
    explicit strong_type( type&& value, typename std::enable_if<!std::is_reference<type_>{ }, std::nullptr_t>::type = nullptr )
    : 
    value_( std::move( value ) ) 
    {   }
 
public:
    type value_;
};

struct count_parameter { };
using count32_t = strong_type<std::uint32_t, count_parameter>;

#endif // LUCIOLE_STRONG_TYPES_HPP