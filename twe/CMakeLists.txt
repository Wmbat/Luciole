#
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
project( twe VERSION 0.0.5 )

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    add_compile_options("-Wall")
#    add_compile_options("-Wextra")
#    add_compile_options("-Wconversion")
#    add_compile_options("-pedantic")
#    add_compile_options("-Wfatal-errors")

endif()

add_library( twe STATIC source/twe/vulkan/core.hpp )

# Declare the library's C++ version
target_compile_features( twe PUBLIC cxx_std_17 )

if( WIN32 )
    target_compile_definitions( twe PUBLIC TWE_PLATFORM_WINDOWS )
    target_compile_definitions( twe PRIVATE TWE_BUILD_DLL )
endif( )

set_target_properties( twe PROPERTIES VERSION ${PROJECT_VERSION} )
set_target_properties( twe PROPERTIES SOVERSION 0 )

# Declare the library's libs.
if( NOT WIN32 )
    if( USE_WAYLAND_WSI )
        target_link_libraries( twe
                ${WAYLAND_CLIENT_LIBRARIES}
                volk
                stb
                spdlog
                nlohmann
                tinygltf
                glm )
    else( )
        target_link_libraries( twe
                ${XCB_LIBRARIES}
                volk
                stb
                spdlog
                nlohmann
                tinygltf
                glm )
    endif()
else()
    target_link_libraries( twe
            volk
            stb
            spdlog
            nlohmann
            tinygltf
            glm )
endif( )

target_sources( twe
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/graphics/mesh.hpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/graphics/renderer.cpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/graphics/renderer.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/graphics/vertex.hpp"

        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/utilities/basic_error.cpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/utilities/basic_error.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/utilities/delegate.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/utilities/file_io.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/utilities/log.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/utilities/message.hpp"

        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/context.hpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/error.cpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/error.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/memory_allocator.hpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/memory_allocator.cpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/pipeline.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/pipeline_layout.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/pipeline_manager.hpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/pipeline_manager.cpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/swapchain.hpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/swapchain.cpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/shader_manager.cpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/shader_manager.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/shader.hpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/utils.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/vertex_buffer.hpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/vertex_buffer.cpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/vulkan/vma_define.cpp"

        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/window/base_window.cpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/window/base_window.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/window/event.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/window/keyboard.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/window/mouse.hpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/window/wayland_window.cpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/window/wayland_window.hpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/window/win32_window.cpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/window/win32_window.hpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/window/xcb_window.cpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/window/xcb_window.hpp"

        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/application.cpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/application.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/entry_point.hpp"
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/twe.hpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/twe_core.hpp"
        )

target_include_directories( twe INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/source" )