/*
 * @author wmbat@protonmail.com
 *
 * Copyright (C) 2018-2019 Wmbat
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * You should have received a copy of the GNU General Public License
 * GNU General Public License for more details.
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LUCIOLE_VULKAN_EXTENSIONS_HPP
#define LUCIOLE_VULKAN_EXTENSIONS_HPP

#include <cstdint>

#include "utils.hpp"

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
            { VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, extension_mode::e_optional },
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
         * @brief = Get the names of the enabled extensions.
         * 
         * @return const std::vector<const char*> - The names of the enabled extensions.
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
         * @brief - Check if all the extensions required by the library are enabled.
         * 
         * @return - true if all required extensions are enabled. 
         * @return - false if a required extension is not enabled. 
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
         * @brief - Enable an extension.
         * 
         * @param [in] name - The name of the extension to enable. 
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

#endif // LUCIOLE_VULKAN_EXTENSIONS_HPP