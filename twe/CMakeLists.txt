add_library( twe SHARED
        source/twe/graphics/pipeline.cpp
        source/twe/graphics/pipeline.h
        source/twe/graphics/pipeline_manager.cpp
        source/twe/graphics/pipeline_manager.h
        source/twe/graphics/shader.cpp
        source/twe/graphics/shader.h
        source/twe/graphics/shader_manager.cpp
        source/twe/graphics/shader_manager.h
        source/twe/utilities/basic_error.cpp
        source/twe/utilities/basic_error.h
        source/twe/utilities/delegate.h
        source/twe/utilities/file_io.h
        source/twe/utilities/vk_error.cpp
        source/twe/utilities/vk_error.h
        source/twe/window/base_window.cpp
        source/twe/window/base_window.hpp
        source/twe/window/wayland_window.cpp
        source/twe/window/wayland_window.hpp
        source/twe/window/win32_window.cpp
        source/twe/window/win32_window.hpp
        source/twe/window/xcb_window.cpp
        source/twe/window/xcb_window.hpp
        source/twe/application.cpp
        source/twe/application.h
        source/twe/entry_point.h
        source/twe/keyboard.h
        source/twe/log.cpp
        source/twe/log.h
        source/twe/message.h
        source/twe/mouse.h
        source/twe/renderer.cpp
        source/twe/renderer.h
        source/twe/TWE.h
        source/twe/TWE_core.h
        source/twe/vertex.h
        source/twe/vk_utils.h ../demo/main.cpp)

if( WIN32 )
    target_compile_definitions( twe PUBLIC TWE_PLATFORM_WINDOWS )
    target_compile_definitions( twe PRIVATE TWE_BUILD_DLL )
endif( )

set_target_properties( twe PROPERTIES VERSION ${PROJECT_VERSION} )
set_target_properties( twe PROPERTIES SOVERSION 1 )

target_include_directories( twe PUBLIC "${CMAKE_SOURCE_DIR}/source" )

if( NOT WIN32 )
    if( USE_WAYLAND_WSI )
        target_link_libraries( twe
                ${Vulkan_LIBRARY}
                ${WAYLAND_CLIENT_LIBRARIES}
                libglslang.a
                libSPIRV.a )
    else( )
        target_link_libraries( twe
                ${Vulkan_LIBRARY}
                ${XCB_LIBRARIES}
                libglslang.a
                libSPIRV.a )
    endif()
else()
    target_link_libraries( twe
            ${Vulkan_LIBRARY} )
endif( )