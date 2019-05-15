#ifndef LUCIOLE_SURFACE_HPP
#define LUCIOLE_SURFACE_HPP

#include "vulkan/volk/volk.h"

#include "window/base_window.hpp"

#include "instance.hpp"

namespace lcl::vulkan
{
    struct surface
    {
        surface( ) = default;
        surface( const base_window& window, const instance& instance );

        VkSurfaceKHR handle_;
    };

} // lcl::vulkan

#endif // LUCIOLE_SURFACE_HPP