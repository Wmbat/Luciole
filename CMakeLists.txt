cmake_minimum_required( VERSION 3.12 )
project( vulkan_project )

set( CMAKE_CXX_STANDARD 17 )

set( GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )

find_package( Vulkan REQUIRED )

if( ${VULKAN_FOUND} )
    include_directories( ${Vulkan_INCLUDE_DIR} )
else( )
    message( ERROR "Vulkan not found!" )
endif( )

find_package( glfw3 3.2 REQUIRED )

if( WIN32 )
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/external/win32/ )
else( )
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/external/linux/ )
endif( )

add_executable( vulkan_project
        test/game.cpp
        test/game.h
        test/main.cpp

        engine/window.cpp
        engine/window.h engine/renderer.cpp engine/renderer.h engine/utilities/file_io.h)

target_link_libraries( vulkan_project
        ${Vulkan_LIBRARY}
        glfw ${GLFW_LIBRARIES} )