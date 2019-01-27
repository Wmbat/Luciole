cmake_minimum_required( VERSION 3.11 )
project( volk )

if( MSVC )
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /EHsc")
endif( )

add_library( volk STATIC include/volk/volk.c )

if(MSVC)
  target_compile_options( volk PRIVATE /W4 /WX )
else()
  target_compile_options( volk PRIVATE -Wall -Wextra -Werror )

  target_link_libraries( volk PRIVATE dl )
endif()

if( DEFINED ENV{VULKAN_SDK})
	target_include_directories( volk PUBLIC "$ENV{VULKAN_SDK}/include" )
endif()

target_include_directories( volk INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include" )
