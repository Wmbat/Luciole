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

#include "swapchain.hpp"

namespace lcl::gfx
{
    swapchain::swapchain( const context* p_context )
        :
        p_context_( p_context )
    {
        auto capabilities = p_context_->get_surface_capabilities( );

        auto format = choose_surface_format( p_context_->get_surface_formats( ) );
        auto present_mode = choose_present_mode( p_context_->get_present_modes( ) );
    } 

    swapchain::swapchain( swapchain&& other )
    {
        *this = std::move( other );
    }
    swapchain::~swapchain( )
    {
        if ( p_context_ )
        {
            p_context_ = nullptr;
        }
    }

    swapchain& swapchain::operator=( swapchain&& rhs )
    {
        if ( this != &rhs )
        {
            p_context_ = rhs.p_context_;
        }

        return *this;
    }

    VkSurfaceFormatKHR swapchain::choose_surface_format( const std::vector<VkSurfaceFormatKHR>& available_formats )
    {
        for( const auto& format : available_formats )
        {
            if ( format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
            {
                return format;
            }
        }

        return available_formats[0];
    }

    VkPresentModeKHR swapchain::choose_present_mode( const std::vector<VkPresentModeKHR>& available_present_modes )
    {
        VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR;

        for( const auto& present_mode : available_present_modes )
        {
            if ( present_mode == VK_PRESENT_MODE_MAILBOX_KHR )
            {
                return present_mode;
            }
            else if ( present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR )
            {
                best_mode = present_mode;
            }
        }

        return best_mode;
    }

    VkExtent2D choose_extent( const VkSurfaceCapabilitiesKHR& capabilities )
    {
        if ( capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max( ) )
        {
            return capabilities.currentExtent;
        }
        else
        {
            VkExtent2D actual_extent = { };
        }
    }
}