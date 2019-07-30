/*!
 * @author wmbat@protonmail.com
 *
 * Copyright (C) 2018-2019 Wmbat
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * You should have received a copy of the GNU General Public License
 * GNU General Public License for more details.
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LUCIOLE_UTILITIES_LOG_HPP
#define LUCIOLE_UTILITIES_LOG_HPP

#include <wmbats_bazaar/logger.hpp>

inline static void core_info( const std::string& msg )
{
#ifndef NDEBUG
    bzr::logger::get_logger().info( msg );
#endif
}
inline static void core_trace( const std::string& msg )
{
#ifndef NDEBUG
    bzr::logger::get_logger().trace( msg );
#endif
}

inline static void core_warn( const std::string& msg )
{
#ifndef NDEBUG
        bzr::logger::get_logger().warn( msg );
#endif
}

inline static void core_critical( const std::string& msg )
{
    bzr::logger::get_logger().critical( msg );
}

inline static void core_error( const std::string& msg )
{
    bzr::logger::get_logger().error( msg );
}

template<typename... arguments>
inline static void core_info( const std::string& msg, const arguments&... args )
{
#ifndef NDEBUG
    bzr::logger::get_logger().info( msg.c_str( ), args... );
#endif
}
template<typename... arguments>
inline static void core_trace( const std::string& msg, const arguments&... args )
{
#ifndef NDEBUG
    bzr::logger::get_logger().trace( msg.c_str( ), args... );
#endif
}
template<typename... arguments>
inline static void core_warn( const std::string& msg, const arguments&... args )
{
#ifndef NDEBUG
    bzr::logger::get_logger().warn( msg.c_str( ), args... );
#endif
}
template<typename... arguments>
inline static void core_critical( const std::string& msg, const arguments&... args )
{
    bzr::logger::get_logger().critical( msg.c_str(), args... );
}

template<typename... arguments>
inline static void core_error( const std::string& msg, const arguments&... args )
{
    bzr::logger::get_logger().error( msg.c_str( ), args... );
}

#endif //LUCIOLE_UTILITIES_LOG_HPP