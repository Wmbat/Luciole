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

cmake_minimum_required( VERSION 3.12 )
project( luciole LANGUAGES CXX )

option( BUILD_TEST "Enable unit testing" OFF )

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release )
endif( )


set( CMAKE_CXX_STANDARD 17 )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )

if( UNIX )
    set( CMAKE_CXX_FLAGS -pthread )
    OPTION( USE_WAYLAND_WSI "Build the project using Wayland swapchain" OFF )
endif( )

if( WIN32 )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR" )        
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR" )
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
        set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_USE_PLATFORM_WAYLAND_KHR" )
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

add_subdirectory( external/vulkan )
add_subdirectory( external/stb )
add_subdirectory( external/glm )
add_subdirectory( external/nlohmann )
add_subdirectory( external/tinygltf )
add_subdirectory( external/wmbats_bazaar )
add_subdirectory( external/googletest )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
    target_compile_options( luciole PUBLIC $<$<CONFIG:DEBUG>:-fno-omit-frame-pointer> )
    target_compile_options( luciole PUBLIC $<$<CONFIG:DEBUG>:-Wall> )
    target_compile_options( luciole PUBLIC $<$<CONFIG:DEBUG>:-o0> )
    target_compile_options( luciole PUBLIC $<$<CONFIG:RELEASE>:-o3> )
elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC" )
	target_compile_features( luciole PUBLIC cxx_std_20 )

	target_compile_definitions( luciole PUBLIC "/EHsc" )
endif( )

add_executable( luciole )

if ( BUILD_TEST )
    message( STATUS "Building unit tests." )
    target_compile_definitions( luciole PUBLIC BUILD_TEST )
endif( )

if( NOT WIN32 )
	target_link_libraries( luciole
	    PUBLIC ${XCB_LIBRARIES}
        PUBLIC vulkan
        PUBLIC stb
        PUBLIC spdlog
        PUBLIC wmbats_bazaar
        PUBLIC nlohmann
        PUBLIC tinygltf
        PUBLIC glm
        PUBLIC gtest )
else()
    target_link_libraries( luciole
        PUBLIC vulkan
        PUBLIC stb
        PUBLIC spdlog
		PUBLIC wmbats_bazaar
        PUBLIC nlohmann
        PUBLIC tinygltf
        PUBLIC glm
		PUBLIC gtest )
endif( )

target_sources( luciole
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/graphics/renderer.cpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/graphics/renderer.hpp"

    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/memory/memory_pool.cpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/memory/memory_pool.hpp"

    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/threads/atomic_queue.hpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/threads/thread_pool.cpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/threads/thread_pool.hpp"

    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/utilities/log.hpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/utilities/message.hpp"

    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/vulkan/queue.cpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/vulkan/queue.hpp"
    
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/window/event.hpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/window/keyboard.hpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/window/mouse.hpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/window/win32_window.cpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/window/win32_window.hpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/window/window.cpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/window/window.hpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/window/xcb_window.cpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/window/xcb_window.hpp"

    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/luciole_core.hpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/application.cpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/application.hpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/context.cpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/context.hpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/extension.hpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/layer.hpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/main.cpp"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/luciole/strong_types.hpp" )

add_custom_target( copy_resources 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/resources
    DEPENDS ${CMAKE_SOURCE_DIR}/resources )

add_dependencies( luciole copy_resources )