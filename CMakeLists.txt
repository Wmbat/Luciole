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

cmake_minimum_required( VERSION 3.12 )
project( vulkan_project )

set( CMAKE_CXX_STANDARD 17 )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )

set( CMAKE_CXX_FLAGS -pthread )

set( GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )


include_directories( external )
include_directories( external/glfw )
include_directories( external/glm )

find_package( glfw3 3.2 REQUIRED )

find_package( Vulkan REQUIRED )

if( UNIX )
    if( NOT Vulkan_FOUND )
        find_library( Vulkan_LIBRARY NAMES vulkan HINTS "$ENV{VULKAN_SDK}/lib" "${CMAKE_CURRENT_SOURCE_DIR}/libs/vulkan" REQUIRED )

        if( Vulkan_LIBRARY )
            set( Vulkan_FOUND ON )
            message( "Using bundled Vulkan library version" )
        endif( )
    endif( )


    find_package( XCB REQUIRED )

    if( NOT XCB_FOUND )
        message( FATAL_ERROR "XCB development package not found" )
    else( )
        message( STATUS ${XCB_LIBRARIES} )
    endif( )

    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_XCB_KHR" )
endif( )





if( NOT Vulkan_FOUND )
    message( FATAL_ERROR "Could not find Vulkan library!" )
else( )
    message( STATUS ${Vulkan_LIBRARY} )
endif( )



find_package( OpenAL REQUIRED )

if( NOT OPENAL_FOUND )
    message( FATAL_ERROR "Could not find OpenAL library! Check if installed" )
else( )
    message( STATUS ${OPENAL_LIBRARY} )
endif()

add_executable( vulkan_project
        test/game.cpp
        test/game.h
        test/main.cpp

        engine/renderer.cpp
        engine/renderer.h
        engine/window.cpp
        engine/window.h
        engine/utilities/file_io.h engine/io/keyboard.h engine/io/keyboard.cpp engine/io/mouse.h engine/io/mouse.cpp engine/vulkan_core.h engine/vulkan_core.cpp engine/vulkan_utils.h engine/console.h engine/console.cpp engine/io/keyboard_layouts.h)

target_link_libraries( vulkan_project
        ${Vulkan_LIBRARY}
        ${XCB_LIBRARIES}
        ${WAYLAND_CLIENT_LIBRARIES}
        glfw ${GLFW_LIBRARIES} )