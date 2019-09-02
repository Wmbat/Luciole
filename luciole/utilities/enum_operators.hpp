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

#ifndef LUCIOLE_UTILITIES_ENUM_OPERATORS
#define LUCIOLE_UTILITIES_ENUM_OPERATORS

#include <type_traits>

#include <type_traits>

template<typename T>
struct enable_bitmask_operators
{
    static_assert( std::is_enum_v<T>, "Template parameter is not of enum type." );

    static constexpr bool enable = false;
};

#define ENABLE_BITMASK_OPERATORS( enum_class )                                          \
template<>                                                                              \  
struct enable_bitmask_operators<enum_class>                                             \  
{                                                                                       \
    static_assert( std::is_enum_v<enum_class>, "Template parameter is not of enum type." );      \
                                                                                        \
    static constexpr bool enable = true;                                                \
};

template<typename T>  
typename std::enable_if_t<enable_bitmask_operators<T>::enable, T> operator|( T lhs, T rhs)  
{
    static_assert( std::is_enum_v<T>, "Template parameter is not of enum type." );

    return static_cast<T> ( static_cast<std::underlying_type_t<T>>( lhs ) | static_cast<std::underlying_type_t<T>>( rhs ) );
}

template<typename T>  
typename std::enable_if_t<enable_bitmask_operators<T>::enable, T> operator&( T lhs, T rhs)  
{
    static_assert( std::is_enum_v<T>, "Template parameter is not of enum type." );

    return static_cast<T> ( static_cast<std::underlying_type_t<T>>( lhs ) & static_cast<std::underlying_type_t<T>>( rhs ) );
}

template<typename T>  
typename std::enable_if_t<enable_bitmask_operators<T>::enable, T> operator^( T lhs, T rhs)  
{
    static_assert( std::is_enum_v<T>, "Template parameter is not of enum type." );

    return static_cast<T> ( static_cast<std::underlying_type_t<T>>( lhs ) | static_cast<std::underlying_type_t<T>>( rhs ) );
}

template<typename T>  
typename std::enable_if_t<enable_bitmask_operators<T>::enable, T> operator~( T rhs )  
{
    static_assert( std::is_enum_v<T>, "Template parameter is not of enum type." );

    return static_cast<T> ( ~static_cast<std::underlying_type_t<T>>( rhs ) );
}

template<typename T>
typename std::enable_if_t<enable_bitmask_operators<T>::enable, T> operator|=( T& lhs, T rhs )
{
    static_assert( std::is_enum_v<T>, "Template parameter is not of enum type." );

    lhs = static_cast<T>( static_cast<std::underlying_type_t<T>>( lhs ) | static_cast<std::underlying_type_t<T>>( rhs ) );

    return lhs;
}

template<typename T>
typename std::enable_if_t<enable_bitmask_operators<T>::enable, T> operator&=( T& lhs, T rhs )
{
    static_assert( std::is_enum_v<T>, "Template parameter is not of enum type." );

    lhs = static_cast<T>( static_cast<std::underlying_type_t<T>>( lhs ) & static_cast<std::underlying_type_t<T>>( rhs ) );

    return lhs;
}

template<typename T>
typename std::enable_if_t<enable_bitmask_operators<T>::enable, T> operator^=( T& lhs, T rhs )
{
    static_assert( std::is_enum_v<T>, "Template parameter is not of enum type." );

    lhs = static_cast<T>( static_cast<std::underlying_type_t<T>>( lhs ) ^ static_cast<std::underlying_type_t<T>>( rhs ) );

    return lhs;
}

#endif // LUCIOLE_UTILITIES_ENUM_OPERATORS