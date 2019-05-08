#ifndef LCL_INSTANCE_HPP
#define LCL_INSTANCE_HPP

#include <vector>

#include "utilities/log.hpp"

#include "vulkan/vulkan.hpp"

#include "extensions.hpp"
#include "layers.hpp"
#include "luciole_core.hpp"

namespace lcl::vulkan
{ 
    struct instance
    {
    public:
        LUCIOLE_API instance( ) = default;
        LUCIOLE_API instance( const std::vector<const char*>& desired_extensions );

        /**
         * @brief - Get a const reference to the vulkan instance handle. Used for cleaner access.
         * 
         * @return - A const reference to the vulkan instance handle.
         */
        LUCIOLE_API const vk::Instance& get( ) const noexcept;
        
        /**
         * @brief - Get a reference to the vulkan instance handle. Used for cleaner access.
         * 
         * @return - A reference to the vulkan instance handle.
         */
        LUCIOLE_API vk::Instance& get( ) noexcept;

    private:
        /**
         * @brief - Check if the current vulkan drivers and Luciole supports the user's desired
         * extensions. It changes the state of the extensions_ struct by enabling the extensions that
         * are supported.
         * 
         * @param [in] desired_extensions - The extension the user wishes to use.
         */
        void check_extension_support( const std::vector<const char*>& desired_extensions );
        /**
         * @brief - Check if the current vulkan drivers and Luciole supports the user's desired
         * extensions. It changes the state of the layers_ struct by enabling the layers that
         * are supported.
         * 
         * @param [in] desired_layers - The layers the user wishes to use.
         */
        void check_layer_support( const std::vector<const char*>& desired_layers );

    public:
        vk::UniqueInstance p_instance_;
        vk::UniqueDebugReportCallbackEXT debug_report_;

        instance_extensions extensions_;
        instance_layers layers_;
    };
}

#endif // LCL_INSTANCE_HPP