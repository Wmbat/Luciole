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
project( vulkan )

find_package( Vulkan )

add_library( vulkan INTERFACE )

if ( NOT Vulkan_FOUND )
    if ( WIN32 )
        find_library( Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${CMAKE_SOURCE_DIR}/lib )

        if ( Vulkan_LIBRARY )
            set ( Vulkan_FOUND ON )
            message( STATUS "Using bundled vulkan library version." )
        endif( )
    else( )
        find_library( Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${CMAKE_SOURCE_DIR}/lib )

        if ( Vulkan_LIBRARY )
            set ( Vulkan_FOUND on )
            message( STATUS "Using bundled vulkan library version." )
        endif( )
    endif( )
endif ( )

target_include_directories( vulkan INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include" )

if ( WIN32 )

else( )
    target_link_libraries( vulkan INTERFACE libvulkan.so )
endif( )