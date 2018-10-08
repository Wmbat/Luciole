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

cmake_minimum_required( VERSION 3.12 )
project( vulkan_project )

set( CMAKE_CXX_STANDARD 17 )

set( CMAKE_CXX_FLAGS -pthread )

set( GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )

find_package( Vulkan REQUIRED )
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

        engine/renderer.cpp
        engine/renderer.h
        engine/window.cpp
        engine/window.h
        engine/utilities/file_io.h engine/keyboard.h engine/keyboard.cpp engine/mouse.h engine/mouse.cpp)

target_link_libraries( vulkan_project
        ${Vulkan_LIBRARY}
        glfw ${GLFW_LIBRARIES} )