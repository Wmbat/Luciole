#  Copyright (C) 2018 Wmbat
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  You should have received a copy of the GNU General Public License
#  GNU General Public License for more details.
#  along with this program. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required( VERSION 3.11 )
project( engine )

set( CMAKE_CXX_STANDARD 17 )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )

if( UNIX )
    set( CMAKE_CXX_FLAGS -pthread )
    OPTION( USE_WAYLAND_WSI "Build the project using Wayland swapchain" OFF )
endif( )

if( WIN32 )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR" )
elseif( UNIX )
    if( USE_WAYLAND_WSI )
        find_package( Wayland REQUIRED )

        if( NOT WAYLAND_FOUND )
            message( FATAL_ERROR "Wayland development package not found" )
        else( )
            message( STATUS ${WAYLAND_CLIENT_LIBRARIES} )
        endif( )

        include_directories( ${WAYLAND_INCLUDE_DIR} )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WAYLAND_KHR")
    else( )
        find_package( XCB REQUIRED )

        if( NOT XCB_FOUND )
            message( FATAL_ERROR "XCB development package not found" )
        else( )
            message( STATUS ${XCB_LIBRARIES} )
        endif( )

        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_XCB_KHR" )
        set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_USE_PLATFORM_XCB_KHR" )
    endif()
endif( )

if( MSVC )
	SET( CMAKE_CXX_FLAGS    "${CMAKE_CXX_FLAGS} /EHsc")
endif( )

add_subdirectory( external/volk )
add_subdirectory( external/stb )
add_subdirectory( external/glm )
add_subdirectory( external/spdlog )
add_subdirectory( external/nlohmann )
add_subdirectory( external/tinygltf )

add_subdirectory( twe )
add_subdirectory( demo )