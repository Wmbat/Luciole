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
project( demo )

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    add_compile_options("-Wall")
    #    add_compile_options("-Wextra")
    #    add_compile_options("-Wconversion")
    #    add_compile_options("-pedantic")
    #    add_compile_options("-Wfatal-errors")

endif()

add_executable( demo )

target_compile_features( twe PUBLIC cxx_std_17 )

if( NOT WIN32 )
    if( USE_WAYLAND_WSI )
        target_link_libraries( demo
                ${WAYLAND_CLIENT_LIBRARIES}
                volk
                stb
                spdlog
                nlohmann
                tinygltf
                glm
                twe )
    else( )
        target_link_libraries( demo
                ${XCB_LIBRARIES}
                volk
                stb
                spdlog
                nlohmann
                tinygltf
                glm
                twe )
    endif()
else()
    target_link_libraries( demo
            volk
            stb
            spdlog
            nlohmann
            tinygltf
            glm
            twe )
endif( )

target_sources( demo
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" )