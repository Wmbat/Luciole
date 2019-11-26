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

// INCLUDES //
#include <cstdint>
#include <string>

#include <luciole/luciole_core.hpp>

struct default_parameter
{
};

template <typename type_, typename parameter_ = default_parameter>
class strong_type
{
public:
   explicit strong_type( const type_& value ) : data( value ) {}

   template <typename type1_ = type_>
   explicit strong_type( type_&& value, typename std::enable_if<!std::is_reference<type1_>{}, std::nullptr_t>::type = nullptr ) :
      data( std::move( value ) )
   {}

   type_ const& value( ) const PURE { return data; }

   type_ value( ) { return data; }

public:
   type_ data;
};

struct count_parameter
{
};
using count32_t = strong_type<std::uint32_t, count_parameter>;

#endif // LUCIOLE_STRONG_TYPES_HPP
