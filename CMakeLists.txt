#  Copyright (C) 2018 BouwnLaw
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

cmake_minimum_required( VERSION 3.9 )
project( TWE VERSION 0.0.2 )

set( CMAKE_CXX_STANDARD 17 )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )

if( UNIX )
    set( CMAKE_CXX_FLAGS -pthread )
    OPTION( USE_WAYLAND_WSI "Build the project using Wayland swapchain" OFF )
endif( )

include_directories( external/include )
include_directories( external/include/glm )
include_directories( external/include/SPIRV )
include_directories( external/include/glslang )
include_directories( external/include/nlohmann )
include_directories( external/include/StandAlone )
include_directories( external/include/spdlog/include )

find_package( Vulkan )

if( WIN32 )
    if( NOT Vulkan_FOUND )
        find_library( Vulkan_LIBRARY NAMES vulkan-1 HINTS "$ENV{VULKAN_SDK}/lib" "external/libs/win32/vulkan" )

        if( Vulkan_LIBRARY )
            set( Vulkan_FOUND ON )
            message( "Using bundled Vulkan library" )
        endif( )
    endif( )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR" )
elseif( UNIX )

    if( NOT Vulkan_FOUND )
        find_library( Vulkan_LIBRARY NAMES libvulkan HINTS  "$ENV{VULKAN_SDK}/lib" "external/libs/linux/vulkan" )

        if( Vulkan_LIBRARY )
            set( Vulkan_FOUND ON )
            message( "Using bundled Vulkan library" )
        endif( )
    endif( )

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
    endif()
endif( )

if( NOT Vulkan_FOUND )
    message( FATAL_ERROR "Could not find Vulkan library!" )
else( )
    message( STATUS ${Vulkan_LIBRARY} )
endif( )


if( MSVC )
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif( )

add_subdirectory( twe )
add_subdirectory( demo )