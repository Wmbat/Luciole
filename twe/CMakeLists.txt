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
project( twe VERSION 0.0.4 )

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    add_compile_options("-Wall")
#    add_compile_options("-Wextra")
#    add_compile_options("-Wconversion")
#    add_compile_options("-pedantic")
#    add_compile_options("-Wfatal-errors")

endif()

add_library( twe SHARED
        source/twe/vulkan/pipeline.cpp
        source/twe/vulkan/pipeline.hpp
        source/twe/vulkan/pipeline_manager.cpp
        source/twe/vulkan/pipeline_manager.hpp
        source/twe/vulkan/shader.cpp
        source/twe/vulkan/shader.hpp
        source/twe/vulkan/shader_manager.cpp
        source/twe/vulkan/shader_manager.hpp
        source/twe/utilities/basic_error.cpp
        source/twe/utilities/basic_error.h
        source/twe/utilities/delegate.hpp
        source/twe/utilities/file_io.hpp
        source/twe/utilities/vk_error.cpp
        source/twe/utilities/vk_error.hpp
        source/twe/window/base_window.cpp
        source/twe/window/base_window.hpp
        source/twe/window/wayland_window.cpp
        source/twe/window/wayland_window.hpp
        source/twe/window/win32_window.cpp
        source/twe/window/win32_window.hpp
        source/twe/window/xcb_window.cpp
        source/twe/window/xcb_window.hpp
        source/twe/application.cpp
        source/twe/application.hpp
        source/twe/entry_point.hpp
        source/twe/window/keyboard.hpp
        source/twe/utilities/log.cpp
        source/twe/utilities/log.hpp
        source/twe/utilities/message.hpp
        source/twe/window/mouse.hpp )

# Declare the library's C++ version
target_compile_features( twe PUBLIC cxx_std_17 )

if( WIN32 )
    target_compile_definitions( twe PUBLIC TWE_PLATFORM_WINDOWS )
    target_compile_definitions( twe PRIVATE TWE_BUILD_DLL )
endif( )

set_target_properties( twe PROPERTIES VERSION ${PROJECT_VERSION} )
set_target_properties( twe PROPERTIES SOVERSION 1 )

# Declare the library's libs.
if( NOT WIN32 )
    if( USE_WAYLAND_WSI )
        target_link_libraries( twe
                ${WAYLAND_CLIENT_LIBRARIES}
                spdlog
                nlohmann
                glm
                vulkan )
    else( )
        target_link_libraries( twe
                ${XCB_LIBRARIES}
                spdlog
                nlohmann
                glm
                vulkan )
    endif()
else()
    target_link_libraries( twe
            spdlog
            nlohmann
            glm
            vulkan )
endif( )

# Declace the library's files.
target_sources( twe
        PRIVATE  "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/graphics/renderer.cpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/graphics/renderer.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/twe.hpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/twe_core.hpp"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/graphics/vertex.hpp"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/twe/utilities/vk_utils.hpp" )

target_include_directories( twe INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/source" )