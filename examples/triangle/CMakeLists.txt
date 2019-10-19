# Copyright (C) 2018-2019 Wmbat
#
# wmbat@protonmail.com
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# You should have received a copy of the GNU General Public License
# GNU General Public License for more details.
# along with this program. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required( VERSION 3.15 )
project( Triangle LANGUAGES CXX )

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release )
endif( )

add_executable( Triangle )

set_target_properties( Triangle PROPERTIES
    DEBUG_POSTFIX "Debug"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples/triangle/bin"
)

set( GNU_VERSION_FLAGS "-std=c++2a" )
set( GNU_DEBUG_FLAGS "-o0 -Wall -Wextra -Werror" )
set( GNU_RELEASE_FLAGS "o3" )
set( GNU_ALL_FLAGS "-fconcepts" )

target_compile_options( Triangle 
    PUBLIC
        $<$<PLATFORM_ID:UNIX>:-pthread>
# Set C++ version
        $<$<CXX_COMPILER_ID:GNU>:${GNU_VERSION_FLAGS}>
        $<$<CXX_COMPILER_ID:MSVC>:-std:c++latest> 
# Set Debug Flags
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:${GNU_DEBUG_FLAGS}>
# Set Release Flags
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:${GNU_RELEASE_FLAGS}>
# All Config flags
        $<$<CXX_COMPILER_ID:GNU>:${GNU_ALL_FLAGS}>
)

target_link_libraries( Triangle
    PRIVATE
        Luciole
)

target_sources( Triangle
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

add_custom_target( copy_resources 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        ${CMAKE_CURRENT_BINARY_DIR}/data
    DEPENDS ${CMAKE_SOURCE_DIR}/data 
)

add_dependencies( Luciole copy_resources )
