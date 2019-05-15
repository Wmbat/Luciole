#ifndef LCL_INSTANCE_HPP
#define LCL_INSTANCE_HPP

#include <vector>

#include "utilities/log.hpp"

#define VK_NO_PROTOTYPE
#include "vulkan/volk/volk.h"

#include "extensions.hpp"
#include "layers.hpp"
#include "luciole_core.hpp"

namespace lcl::vulkan
{ 
    struct instance
    {
    public:
        LUCIOLE_API instance( );
        LUCIOLE_API instance( const instance& other ) = delete;
        LUCIOLE_API instance( instance&& other ) noexcept;

        LUCIOLE_API instance& operator=( const instance& rhs ) = delete;
        LUCIOLE_API instance& operator=( instance&& rhs ) noexcept;

    private:
        /**
         * @brief - Check if the current vulkan drivers and Luciole supports the user's desired
         * extensions. It changes the state of the layers_ struct by enabling the layers that
         * are supported.
         * 
         * @param [in] desired_layers - The layers the user wishes to use.
         */
        void check_layer_support( const std::vector<const char*>& desired_layers );

    public:
        VkInstance handle_;
        VkDebugReportCallbackEXT debug_report_callback_;

        instance_extensions extensions_;
        instance_layers layers_;
    };
}

#endif // LCL_INSTANCE_HPP