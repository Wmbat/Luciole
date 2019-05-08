#include "surface.hpp"

namespace lcl::vulkan
{
    surface::surface( const base_window& window, const instance& instance )
    {
        p_surface_ = window.create_surface( instance.get( ) );
    }

    const vk::SurfaceKHR& surface::get( ) const noexcept
    {
        return p_surface_.get( );
    }

    vk::SurfaceKHR& surface::get( ) noexcept
    {
        return p_surface_.get( );
    }

} // lcl::vulkan