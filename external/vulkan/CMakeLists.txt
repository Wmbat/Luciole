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
#

cmake_minimum_required( VERSION 3.11 )
project( vulkan )

add_library( vulkan STATIC IMPORTED GLOBAL ) # The Vulkan library is already built outside of the CMake project. We'll import it as a CMake target for convenience.

if( WIN32 )
    target_include_directories(vulkan INTERFACE  "${CMAKE_CURRENT_SOURCE_DIR}/win32/include") # Anyone that uses this library will have these headers.
    set_target_properties(vulkan PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/win32/Libs/vulkan-1.lib")
elseif(APPLE)
    target_include_directories(vulkan INTERFACE macos/macOS/include) # Anyone that uses this library will have these headers.
    set_target_properties(vulkan PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/macos/macOS/lib/libvulkan.1.1.92") # Set the location of the imported library.
else()
    target_include_directories( vulkan INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/linux/include" )
    set_target_properties( vulkan PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/linux/libs/libvulkan.so.1.1.92" )
endif()