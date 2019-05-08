#ifndef LUCIOLE_SURFACE_HPP
#define LUCIOLE_SURFACE_HPP

#include "vulkan/vulkan.hpp"

#include "window/base_window.hpp"

#include "instance.hpp"

namespace lcl::vulkan
{
    struct surface
    {
        surface( ) = default;
        surface( const base_window& window, const instance& instance );

        /**
         * @brief - Get a const reference to the vulkan surface handle. Used for cleaner access
         * 
         * @return - A const reference to the vulkan surface handle.
         */
        const vk::SurfaceKHR& get( ) const noexcept;
        
        /**
         * @brief - Get a const reference to the vulkan surface handle. Used for cleaner access
         * 
         * @return - A reference to the vulkan surface handle.
         */
        vk::SurfaceKHR& get( ) noexcept;

        vk::UniqueSurfaceKHR p_surface_;
    };

} // lcl::vulkan

#endif // LUCIOLE_SURFACE_HPP