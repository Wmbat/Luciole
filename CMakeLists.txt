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
option( BUILD_EXAMPLE "Build demo examples" OFF )

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release )
endif( )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )

add_library( Luciole SHARED )

set_target_properties( Luciole PROPERTIES
    DEBUG_POSTFIX "Debug"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/"
)

target_compile_options( Luciole 
    PUBLIC
        $<$<PLATFORM_ID:UNIX>:-pthread>
# Set C++ version
        $<$<CXX_COMPILER_ID:GNU>:-std=c++2a>
        $<$<CXX_COMPILER_ID:MSVC>:-std:c++latest>
# Set Flags
        $<$<CXX_COMPILER_ID:GNU>:-fconcepts>
# Set Debug Flags
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-o0>
        $<$<AND:$<CXX_COMPILER_ID:GUN>,$<CONFIG:DEBUG>>:-g>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-Wall>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-Wextra>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-Wold-style-cast>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-Wdouble-promotion>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-Werror>
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
            message( STATUS "Using XCB Library" )
        endif( )

        target_compile_definitions( Luciole PUBLIC VK_USE_PLATFORM_XCB_KHR )
    endif()
endif( )

add_subdirectory( external/vulkan )
add_subdirectory( external/vma )
add_subdirectory( external/stb )
add_subdirectory( external/glm )
add_subdirectory( external/nlohmann )
add_subdirectory( external/tinygltf )
add_subdirectory( external/wmbats_bazaar )

if ( BUILD_TEST )
    message( STATUS "Building unit tests." )
    target_compile_definitions( Luciole PUBLIC BUILD_TEST )
endif( )

if( NOT WIN32 )
    target_link_libraries( Luciole
        PUBLIC 
            ${XCB_LIBRARIES}
            vulkan
            vma
            stb
            spdlog
            wmbats_bazaar
            nlohmann
            tinygltf
            glm
        )
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
		)
endif( )

target_include_directories( Luciole 
    PUBLIC 
        "includes" 
)

target_sources( Luciole
    PRIVATE
        "src/luciole/graphics/renderer.cpp"
        "src/luciole/threads/thread_pool.cpp"
        "src/luciole/ui/window.cpp"
        "src/luciole/vk/queue.cpp"
        "src/luciole/context.cpp"
)

add_subdirectory( examples/triangle )