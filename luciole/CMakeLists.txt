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
project( luciole VERSION 0.0.6 )

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release )
endif( )

add_library( luciole SHARED )

# Declare the library's C++ version
target_compile_features( luciole PUBLIC cxx_std_17 )

if( WIN32 )
    target_compile_definitions( luciole PUBLIC LUCIOLE_PLATFORM_WINDOWS )
    target_compile_definitions( luciole PUBLIC LUCIOLE_BUILD_DLL )
endif( )

set_target_properties( luciole PROPERTIES VERSION ${PROJECT_VERSION} )
set_target_properties( luciole PROPERTIES SOVERSION 0 )

# Declare the library's libs.
if( NOT WIN32 )
    if( USE_WAYLAND_WSI )
        target_link_libraries( luciole
                ${WAYLAND_CLIENT_LIBRARIES}
                dl
                vulkan
                stb
                spdlog
                nlohmann
                tinygltf
                glm )
    else( )
        target_link_libraries( luciole
                PUBLIC ${XCB_LIBRARIES}
                PUBLIC dl
                PUBLIC vulkan
                PUBLIC stb
                PUBLIC spdlog
                PUBLIC wmbats_bazaar
                PUBLIC nlohmann
                PUBLIC tinygltf
                PUBLIC glm )
    endif()
else()
    target_link_libraries( luciole
           PUBLIC vulkan
           PUBLIC stb
           PUBLIC spdlog
           PUBLIC nlohmann
           PUBLIC tinygltf
           PUBLIC glm )
endif( )

target_sources( luciole
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/graphics/mesh.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/graphics/renderer.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/graphics/renderer.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/graphics/uniform_buffers.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/graphics/uniform_buffers.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/graphics/vertex.hpp"

        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/utilities/log.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/utilities/message.hpp"

        # vma
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/vma/vk_mem_alloc.h"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/vma/vma_define.cpp"

        # volk
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/volk/volk.c"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/volk/volk.h"

        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/context.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/context.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/error.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/error.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/memory_allocator.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/memory_allocator.hpp"
		PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/mesh_buffer.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/mesh_buffer.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/pipeline.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/pipeline.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/pipeline_layout.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/pipeline_manager.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/pipeline_manager.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/swapchain.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/swapchain.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/shader_manager.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/shader_manager.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/shader.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/vulkan/utils.hpp"

        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/window/base_window.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/window/base_window.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/window/event.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/window/keyboard.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/window/mouse.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/window/wayland_window.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/window/wayland_window.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/window/win32_window.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/window/win32_window.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/window/xcb_window.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/window/xcb_window.hpp"

        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/luciole_core.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/luciole/luciole.hpp"
        )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
    target_compile_options( luciole PUBLIC $<$<CONFIG:DEBUG>:-Wall> )
    target_compile_options( luciole PUBLIC $<$<CONFIG:RELEASE>:-o3> )
endif( )

target_include_directories( luciole INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/source" )