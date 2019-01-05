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

add_executable( demo )

if( NOT WIN32 )
    if( USE_WAYLAND_WSI )
        target_link_libraries( demo
                ${WAYLAND_CLIENT_LIBRARIES}
                spdlog
                nlohmann
                glm
                vulkan
                twe )
    else( )
        target_link_libraries( demo
                ${XCB_LIBRARIES}
                spdlog
                nlohmann
                glm
                vulkan
                twe )
    endif()
else()
    target_link_libraries( demo
            spdlog
            nlohmann
            glm
            vulkan
            twe )
endif( )

target_sources( demo
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" )