#ifndef LUCIOLE_EXTENSIONS_HPP
#define LUCIOLE_EXTENSIONS_HPP

#include <cstdint>

#include "vulkan/vulkan.hpp"

namespace lcl::vulkan
{
    enum class extension_mode
    {
        e_required,
        e_optional,
        e_disabled
    };

    struct extension
    {
        const char* name_ = nullptr;
        extension_mode mode_ = extension_mode::e_disabled;
        bool enabled_ = false;
    };

    struct instance_extensions
    {
        inline const std::vector<const char*> get_enabled_extension_names( ) const
        {
            std::vector<const char*> names;
            names.reserve( extensions_.size( ) );

            for ( const auto& extension : extensions_ )
            {
                if ( extension.enabled_ )
                    names.emplace_back( extension.name_ );
            }

            names.shrink_to_fit( );

            return names;
        }

        inline bool is_core_enabled( ) const
        {
            for ( const auto& extension : extensions_ )
            {
                if ( extension.mode_ == extension_mode::e_required )
                {
                    if ( extension.enabled_ == false )
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        inline void enable( const char* name )
        {
            for ( auto& extension : extensions_ )
            {
                if ( strcmp( extension.name_, name ) == 0 && extension.mode_ != extension_mode::e_disabled )
                {
                    extension.enabled_ = true;
                }
            }
        }

        std::vector<extension> extensions_ = {
            { VK_EXT_DEBUG_REPORT_EXTENSION_NAME, extension_mode::e_optional },
            { VK_KHR_SURFACE_EXTENSION_NAME, extension_mode::e_required },
#if defined( VK_USE_PLATFORM_WIN32_KHR )
            { VK_KHR_WIN32_SURFACE_EXTENSION_NAME, extension_mode::e_required }
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
            { VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME, extension_mode::e_required }
#elif defined( VK_USE_PLATFORM_XCB_KHR )
            { VK_KHR_XCB_SURFACE_EXTENSION_NAME, extension_mode::e_required }
#endif
        };
    };

    struct device_extensions
    {
        /**
         * @brief Get the enabled extension names object
         * 
         * @return const std::vector<const char*> 
         */
        inline const std::vector<const char*> get_enabled_extension_names( ) const
        {
            std::vector<const char*> names;
            names.reserve( extensions_.size( ) );

            for ( const auto& extension : extensions_ )
            {
                if ( extension.enabled_ )
                    names.emplace_back( extension.name_ );
            }

            names.shrink_to_fit( );

            return names;
        }

        /**
         * @brief Check if all the extensions required by the library are enabled.
         * 
         * @return true 
         * @return false 
         */
        inline bool is_core_enabled( ) const
        {
            for ( const auto& extension : extensions_ )
            {
                if ( extension.mode_ == extension_mode::e_required )
                {
                    if ( extension.enabled_ == false )
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        /**
         * @brief 
         * 
         * @param [in] - name 
         */
        inline void enable( const char* name )
        {
            for ( auto& extension : extensions_ )
            {
                if ( strcmp( extension.name_, name ) == 0 && extension.mode_ != extension_mode::e_disabled )
                {
                    extension.enabled_ = true;
                }
            }
        }

        std::vector<extension> extensions_ = {
            { VK_KHR_SWAPCHAIN_EXTENSION_NAME, extension_mode::e_required }
        };
    };
};

#endif // LUCIOLE_EXTENSIONS_HPP