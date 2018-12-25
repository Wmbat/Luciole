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
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif( )

add_library( TWE SHARED
        source/utilities/basic_error.cpp
        source/utilities/vk_error.cpp
		source/window/base_window.cpp
        source/window/wayland_window.cpp
		source/window/win32_window.cpp
        source/window/xcb_window.cpp
		source/application.cpp
        source/log.cpp
        source/renderer.cpp
        source/vk_pipeline_manager.cpp
        source/vk_shader.cpp source/vk_shader_manager.cpp)

if( WIN32 )
	target_compile_definitions( TWE PUBLIC TWE_PLATFORM_WINDOWS )
	target_compile_definitions( TWE PRIVATE TWE_BUILD_DLL )
endif( )

set_target_properties( TWE PROPERTIES VERSION ${PROJECT_VERSION} )
set_target_properties( TWE PROPERTIES SOVERSION 1 )

target_include_directories( TWE PUBLIC TWE )

if( NOT WIN32 )
    if( USE_WAYLAND_WSI )
        target_link_libraries( TWE
                ${Vulkan_LIBRARY}
                ${WAYLAND_CLIENT_LIBRARIES}
                libglslang.a
                libSPIRV.a )
    else( )
        target_link_libraries( TWE
                ${Vulkan_LIBRARY}
                ${XCB_LIBRARIES}
                libglslang.a
                libSPIRV.a )
    endif()
else()
    target_link_libraries( TWE
            ${Vulkan_LIBRARY}
            libglslang.a
            libSPIRV.a )
endif( )