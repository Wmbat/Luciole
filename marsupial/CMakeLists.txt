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
project( marsupial VERSION 0.0.6 )

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    add_compile_options("-Wall")
#    add_compile_options("-Wextra")
#    add_compile_options("-Wconversion")
#    add_compile_options("-pedantic")
#    add_compile_options("-Wfatal-errors")

endif()

add_library( marsupial SHARED )

# Declare the library's C++ version
target_compile_features( marsupial PUBLIC cxx_std_17 )

if( WIN32 )
    target_compile_definitions( marsupial PUBLIC MARSUPIAL_PLATFORM_WINDOWS )
    target_compile_definitions( marsupial PUBLIC MARSUPIAL_BUILD_DLL )
endif( )

set_target_properties( marsupial PROPERTIES VERSION ${PROJECT_VERSION} )
set_target_properties( marsupial PROPERTIES SOVERSION 0 )

# Declare the library's libs.
if( NOT WIN32 )
    if( USE_WAYLAND_WSI )
        target_link_libraries( marsupial
                ${WAYLAND_CLIENT_LIBRARIES}
                dl
                vulkan
                stb
                spdlog
                nlohmann
                tinygltf
                glm )
    else( )
        target_link_libraries( marsupial
                PUBLIC ${XCB_LIBRARIES}
                PUBLIC dl
                PUBLIC vulkan
                PUBLIC stb
                PUBLIC spdlog
                PUBLIC nlohmann
                PUBLIC tinygltf
                PUBLIC glm )
    endif()
else()
    target_link_libraries( marsupial
           PUBLIC vulkan
           PUBLIC stb
           PUBLIC spdlog
           PUBLIC nlohmann
           PUBLIC tinygltf
           PUBLIC glm )
endif( )

target_sources( marsupial
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/graphics/mesh.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/graphics/renderer.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/graphics/renderer.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/graphics/vertex.hpp"

        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/utilities/basic_error.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/utilities/basic_error.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/utilities/delegate.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/utilities/file_io.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/utilities/log.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/utilities/message.hpp"

        # vma
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/vma/vk_mem_alloc.h"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/vma/vma_define.cpp"

        # volk
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/volk/volk.c"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/volk/volk.h"

        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/buffer.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/context.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/error.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/error.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/memory_allocator.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/memory_allocator.hpp"
		PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/mesh_buffer.cpp"
		PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/mesh_buffer.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/pipeline.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/pipeline_layout.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/pipeline_manager.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/pipeline_manager.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/swapchain.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/swapchain.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/shader_manager.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/shader_manager.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/shader.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/vulkan/utils.hpp"

        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/window/base_window.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/window/base_window.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/window/event.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/window/keyboard.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/window/mouse.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/window/wayland_window.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/window/wayland_window.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/window/win32_window.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/window/win32_window.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/window/xcb_window.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/window/xcb_window.hpp"

        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/application.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/application.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/entry_point.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/marsupial.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/marsupial/marsupial_core.hpp"
        )

target_include_directories( marsupial INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/source" )