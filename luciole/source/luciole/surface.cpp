#include "surface.hpp"

namespace lcl::vulkan
{
    surface::surface( const base_window& window, const instance& instance )
    {
        handle_ = window.create_surface( instance.handle_ );
    }

} // lcl::vulkan