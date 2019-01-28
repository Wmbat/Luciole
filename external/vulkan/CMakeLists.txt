cmake_minimum_required( VERSION 3.11 )
project( vulkan )

add_library( vulkan INTERFACE )

target_include_directories( vulkan INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include" )