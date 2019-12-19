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

option( BUILD_TESTS "Enable unit testing" OFF )
option( BUILD_EXAMPLES "Build demo examples" ON )

if( NOT CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE Release )
endif( )

# nlohmann settings
set( JSON_BuildTests OFF CACHE BOOL "" FORCE )

# spirv_cross settings
set( SPIRV_CROSS_SHARED OFF CACHE BOOL "" FORCE )
set( SPIRV_CROSS_STATIC ON CACHE BOOL "" FORCE )
set( SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE )
set( SPIRV_CROSS_ENABLE_MSL OFF CACHE BOOL "" FORCE )
set( SPIRV_CROSS_ENABLE_HLSL OFF CACHE BOOL "" FORCE )
set( SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

set( CLANG_FLAGS "-std=c++2a" )
set( CLANG_DEBUG_FLAGS "-o0 -g -Wall -Wextra -Werror -Wdouble-promotion -fsanitize=address -fsanitize=undefined" )
set( CLANG_DEBUG_FLAGS "${CLANG_DEBUG_FLAGS} -Wunused-parameter -Wunused-value  -Wunused-variable" )
set( CLANG_DEBUG_FLAGS "${CLANG_DEBUG_FLAGS} -Wunused-but-set-parameter -Wunused-but-set-variable -fsanitize=address" )
set( CLANG_RELEASE_FLAGS "-o3" )

set( GNU_FLAGS "-std=c++2a" )
set( GNU_DEBUG_FLAGS "-o0 -g -Wall -Wextra -Wdouble-promotion -Werror -fno-omit-frame-pointer" )
set( GNU_DEBUG_FLAGS "${GNU_DEBUG_FLAGS} -Wunused-parameter -Wunused-value  -Wunused-variable" )
set( GNU_DEBUG_FLAGS "${GNU_DEBUG_FLAGS} -Wunused-but-set-parameter -Wunused-but-set-variable -fsanitize=address" )
set( GNU_RELEASE_FLAGS "-o3" )

set( MSVC_FLAGS "-std:c++latest" )

add_library( Luciole STATIC )

set_target_properties( Luciole PROPERTIES
   DEBUG_POSTFIX "Debug"
   LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/"
   ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/"
)

target_compile_options( Luciole 
   PRIVATE
      $<$<PLATFORM_ID:UNIX>:-pthread>

      $<$<CXX_COMPILER_ID:GNU>:${GNU_FLAGS}>
      $<$<CXX_COMPILER_ID:Clang>:${CLANG_FLAGS}>
      $<$<CXX_COMPILER_ID:MSVC>:${MSVC_FLAGS}>
# Set Debug Flags
      $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:${GNU_DEBUG_FLAGS}>
      $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:DEBUG>>:${CLANG_DEBUG_FLAGS}>
# Set Release Flags
      $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:${GNU_RELEASE_FLAGS}>
      $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:RELEASE>>:${CLANG_RELEASE_FLAGS}>
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
add_subdirectory( external/spdlog )
add_subdirectory( external/glm )
add_subdirectory( external/nlohmann )
add_subdirectory( external/tinygltf )
add_subdirectory( external/glslang )
add_subdirectory( external/spirv_cross )

if ( BUILD_TESTS )
   add_subdirectory( external/googletest )
endif( BUILD_TESTS )

if( NOT WIN32 )
   target_link_libraries( Luciole
      PUBLIC 
         ${XCB_LIBRARIES} vulkan vma
         stb spdlog_header_only
         nlohmann_json tinygltf
         glm glslang SPIRV
         spirv-cross-cpp
   )
endif( )

target_include_directories( Luciole 
   PUBLIC 
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
)

target_sources( Luciole
   PRIVATE
      "src/luciole/graphics/renderer.cpp"
      "src/luciole/threads/thread_pool.cpp"
      "src/luciole/ui/window.cpp"
      "src/luciole/utils/logger.cpp"
      "src/luciole/vk/buffers/index_buffer.cpp"
      "src/luciole/vk/buffers/uniform_buffer.cpp"
      "src/luciole/vk/buffers/vertex_buffer.cpp"
      "src/luciole/vk/pipelines/pipeline.cpp"
      "src/luciole/vk/pipelines/pipeline_json_loader.cpp"
      "src/luciole/vk/pipelines/pipeline_manager.cpp"
      "src/luciole/vk/shaders/shader.cpp"
      "src/luciole/vk/shaders/shader_compiler.cpp"
      "src/luciole/vk/shaders/shader_manager.cpp"
      "src/luciole/vk/shaders/shader_set.cpp"
      "src/luciole/vk/descriptor_pool.cpp"
      "src/luciole/vk/queue.cpp"
      "src/luciole/vk/queue_handler.cpp"
      "src/luciole/vk/errors.cpp"
      "src/luciole/vk/vma_define.cpp"
      "src/luciole/context.cpp"
)

if ( BUILD_EXAMPLES )
   add_subdirectory( examples/triangle )
endif ( BUILD_EXAMPLES )
