cmake_minimum_required( VERSION 3.11 )
project( stb )

add_library( stb INTERFACE )

target_include_directories( stb INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include" )