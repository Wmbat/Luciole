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
project( Luciole LANGUAGES CXX )

option( BUILD_TEST "Enable unit testing" OFF )

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release )
endif( )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )

add_subdirectory( external/vulkan )
add_subdirectory( external/stb )
add_subdirectory( external/glm )
add_subdirectory( external/nlohmann )
add_subdirectory( external/tinygltf )
add_subdirectory( external/wmbats_bazaar )
add_subdirectory( external/googletest )

add_executable( Luciole )

set_target_properties( Luciole PROPERTIES
    DEBUG_POSTFIX "_d"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_compile_options( Luciole 
    PUBLIC
        $<$<PLATFORM_ID:UNIX>:-pthread>
# Set C++ version
        $<$<CXX_COMPILER_ID:GNU>:-std=c++17>
        $<$<CXX_COMPILER_ID:MSVC>:-std:c++latest> 
# Set Debug Flags
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-o0>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-Wall>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-fno-omit-frame-pointer>
# Set Release Flags
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:-o3>
)

if( WIN32 )
    target_compile_definitions( Luciole PUBLIC -VK_USE_PLATFORM_WIN32_KHR )
elseif( UNIX )
    if( USE_WAYLAND_WSI )
        find_package( Wayland REQUIRED )

        if( NOT WAYLAND_FOUND )
            message( FATAL_ERROR "Wayland development package not found" )
        else( )
            message( STATUS ${WAYLAND_CLIENT_LIBRARIES} )
        endif( )

        target_include_directories( Luciole PUBLIC ${WAILAND_INCLUDE_DIR} )
        target_compile_definitions( Luciole PUBLIC VK_USE_PLATFORM_WAYLAND_KHR )
    else( )
        find_package( XCB REQUIRED )

        if( NOT XCB_FOUND )
            message( FATAL_ERROR "XCB development package not found" )
        else( )
            message( STATUS ${XCB_LIBRARIES} )
        endif( )

        target_compile_definitions( Luciole PUBLIC VK_USE_PLATFORM_XCB_KHR )
    endif()
endif( )

if ( BUILD_TEST )
    message( STATUS "Building unit tests." )
    target_compile_definitions( Luciole PUBLIC BUILD_TEST )
endif( )

if( NOT WIN32 )
    target_link_libraries( Luciole
        PUBLIC 
            ${XCB_LIBRARIES}
            vulkan
            stb
            spdlog
            wmbats_bazaar
            nlohmann
            tinygltf
            glm
            gtest )
else()
    target_link_libraries( Luciole
        PUBLIC 
            vulkan
            stb
            spdlog
		    wmbats_bazaar
            nlohmann
            tinygltf
            glm
		    gtest )
endif( )

target_sources( Luciole
    PRIVATE
        "src/graphics/renderer.cpp"
        "src/memory/memory_pool.cpp"
        "src/threads/thread_pool.cpp"
        "src/vulkan/queue.cpp"
        "src/window/win32_window.cpp"
        "src/window/window.cpp"
        "src/window/xcb_window.cpp"
        "src/application.cpp"
        "src/context.cpp"
        "src/main.cpp"
)

target_include_directories( Luciole 
    PUBLIC 
        "includes" 
    PRIVATE
        "src"
)

add_custom_target( copy_resources 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        ${CMAKE_BINARY_DIR}/data
    DEPENDS ${CMAKE_SOURCE_DIR}/data )

add_dependencies( Luciole copy_resources )