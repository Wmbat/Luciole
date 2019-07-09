/*
 *  Copyright (C) 2018-2019 Wmbat
 *
 *  wmbat@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  You should have received a copy of the GNU General Public License
 *  GNU General Public License for more details.
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LUCIOLE_GRAPHICS_SWAPCHAIN_HPP
#define LUCIOLE_GRAPHICS_SWAPCHAIN_HPP

#include "context.hpp"

namespace lcl::gfx
{
    class swapchain
    {
    public:
        LUCIOLE_API swapchain( ) = default;
        LUCIOLE_API swapchain( const context* p_context );
        LUCIOLE_API swapchain( const swapchain& other ) = delete;
        LUCIOLE_API swapchain( swapchain&& other );
        LUCIOLE_API ~swapchain( );

        LUCIOLE_API swapchain& operator=( const swapchain& rhs ) = delete;
        LUCIOLE_API swapchain& operator=( swapchain&& rhs );

    private:
        VkSurfaceFormatKHR choose_surface_format( const std::vector<VkSurfaceFormatKHR>& available_formats );
        VkPresentModeKHR choose_present_mode( const std::vector<VkPresentModeKHR>& available_present_modes );
        VkExtent2D choose_extent( const VkSurfaceCapabilitiesKHR& capabilities );

    private:
        const context* p_context_ = nullptr;
    };
} // namespace lcl::gfx

#endif // LUCIOLE_GRAPHICS_SWAPCHAIN_HPP