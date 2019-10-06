cmake_minimum_required(VERSION 3.15)
project( vma )

message( STATUS "Building VMA Header Library" )

add_library( vma INTERFACE )

target_compile_options( vma 
    INTERFACE
# Set Debug Flags
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-o0>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-w>
# Set Release Flags
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:-o3>
)

target_include_directories( vma INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include" )