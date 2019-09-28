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

cmake_minimum_required( VERSION 3.11 )
project( nlohmann )

message( STATUS "Building Nlohmann Header Library" )

add_library( nlohmann INTERFACE )

target_compile_options( nlohmann 
    INTERFACE
# Set C++ version
        $<$<CXX_COMPILER_ID:GNU>:-std=c++17>
        $<$<CXX_COMPILER_ID:MSVC>:-std:c++latest>
# Set Debug Flags
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-o0>
# Set Release Flags
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:-o3>
)

target_include_directories( nlohmann INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include" )