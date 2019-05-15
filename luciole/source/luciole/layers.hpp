#ifndef LUCIOLE_LAYERS_HPP
#define LUCIOLE_LAYERS_HPP

#include "vulkan/volk/volk.h"

namespace lcl::vulkan
{
    enum class layer_mode
    {
        e_required,
        e_optional,
        e_disabled
    };

    struct layer
    {
        const char* name_ = nullptr;
        layer_mode mode_ = layer_mode::e_disabled;
        bool enabled_ = false;
    };

    struct instance_layers
    {
        std::vector<const char*> get_enabled_layers( ) const
        {
            std::vector<const char*> layers;

            if ( lunarg_standard_validation_.enabled_ )
                layers.emplace_back( lunarg_standard_validation_.name_ );

            return layers;
        }

        void enable( const char* name )
        {
            if ( strcmp( lunarg_standard_validation_.name_, name ) == 0 && lunarg_standard_validation_.mode_ != layer_mode::e_disabled )
                lunarg_standard_validation_.enabled_ = true;
        }

        layer lunarg_standard_validation_{ "VK_LAYER_LUNARG_standard_validation", layer_mode::e_optional };
    };
}

#endif // LUCIOLE_LAYERS_HPP