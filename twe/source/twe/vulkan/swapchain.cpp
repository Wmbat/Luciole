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

namespace twe::vulkan
{
    swapchain::swapchain( const create_info_type& create_info )
        :
        width_( create_info.width_ ),
        height_( create_info.height_ )
    {
        const auto support_details = support_details_type( )
            .set_capabilities( create_info.gpu_.getSurfaceCapabilitiesKHR( create_info.surface_ ))
            .set_formats( create_info.gpu_.getSurfaceFormatsKHR( create_info.surface_ ))
            .set_present_modes( create_info.gpu_.getSurfacePresentModesKHR( create_info.surface_ ));
        
        extent_ = choose_extent( support_details.capabilities_ );
        surface_format_ = choose_surface_format( support_details.formats_ );
        const auto present_mode = choose_present_mode( support_details.present_modes_ );
        
        image_count_ = support_details.capabilities_.minImageCount + 1;
        if ( support_details.capabilities_.maxImageCount > 0 &&
             image_count_ > support_details.capabilities_.maxImageCount )
        {
            image_count_ = support_details.capabilities_.maxImageCount;
        }
        
        image_views_.resize( image_count_ );
        framebuffers_.resize( image_count_ );
        
        swapchain_ = create_handle<vk::UniqueSwapchainKHR>( create_info.device_, create_info.surface_, present_mode, support_details.capabilities_ );
        images_ = create_info.device_.getSwapchainImagesKHR( swapchain_.get( ));
        
        for ( std::uint32_t i = 0; i < image_count_; ++i )
        {
            image_views_[i] = create_handle<vk::UniqueImageView>( create_info.device_, images_[i] );
            framebuffers_[i] = create_handle<vk::UniqueFramebuffer>( create_info.device_, create_info.render_pass_, image_views_[i].get( ));
        }
    }
    
    swapchain::swapchain( swapchain&& rhs )
    {
        *this = std::move( rhs );
    }
    
    swapchain& swapchain::operator=( swapchain&& rhs )
    {
        if ( this != &rhs )
        {
            width_ = rhs.width_;
            rhs.width_ = 0;
            
            height_ = rhs.height_;
            rhs.height_ = 0;
            
            image_count_ = rhs.image_count_;
            rhs.image_count_ = 0;
            
            extent_ = std::move( rhs.extent_ );
            
            swapchain_ = std::move( rhs.swapchain_ );
            images_ = std::move( rhs.images_ );
            image_views_ = std::move( rhs.image_views_ );
            framebuffers_ = std::move( rhs.framebuffers_ );
        }
        
        return *this;
    }
    
    void swapchain::recreate( const create_info_type& create_info )
    {
        width_ = create_info.width_;
        height_ = create_info.height_;
        
        create_info.device_.waitIdle( );
        
        for ( auto& framebuffer : framebuffers_ )
        {
            auto p = framebuffer.release( );
            
            create_info.device_.destroyFramebuffer( p );
        }
        for ( auto& image_view : image_views_ )
        {
            auto p = image_view.release( );
            
            create_info.device_.destroyImageView( p );
        }
        
        auto p = swapchain_.release( );
        create_info.device_.destroySwapchainKHR( p );
        
        
        const auto support_details = support_details_type( )
            .set_capabilities( create_info.gpu_.getSurfaceCapabilitiesKHR( create_info.surface_ ) )
            .set_formats( create_info.gpu_.getSurfaceFormatsKHR( create_info.surface_ ) )
            .set_present_modes( create_info.gpu_.getSurfacePresentModesKHR( create_info.surface_ ) );
        
        extent_ = choose_extent( support_details.capabilities_ );
        surface_format_ = choose_surface_format( support_details.formats_ );
        const auto present_mode = choose_present_mode( support_details.present_modes_ );
        
        image_count_ = support_details.capabilities_.minImageCount + 1;
        if ( support_details.capabilities_.maxImageCount > 0 &&
             image_count_ > support_details.capabilities_.maxImageCount )
        {
            image_count_ = support_details.capabilities_.maxImageCount;
        }
        
        image_views_.resize( image_count_ );
        framebuffers_.resize( image_count_ );
        
        swapchain_ = create_handle<vk::UniqueSwapchainKHR>( create_info.device_, create_info.surface_, present_mode, support_details.capabilities_ );
        images_ = create_info.device_.getSwapchainImagesKHR( swapchain_.get( ));
        
        for ( std::uint32_t i = 0; i < image_count_; ++i )
        {
            image_views_[i] = create_handle<vk::UniqueImageView>( create_info.device_, images_[i] );
            framebuffers_[i] = create_handle<vk::UniqueFramebuffer>( create_info.device_, create_info.render_pass_, image_views_[i].get( ));
        }
    }
    
    
    const vk::SurfaceFormatKHR swapchain::choose_surface_format( const std::vector<vk::SurfaceFormatKHR>& available_formats ) const noexcept
    {
        if ( available_formats.size( ) == 1 && available_formats[0].format == vk::Format::eUndefined )
        {
            return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
        }
        
        const auto iter =
            std::find_if( available_formats.cbegin( ), available_formats.cend( ),
                          []( const vk::SurfaceFormatKHR& format )
                          {
                              return format.format == vk::Format::eB8G8R8A8Unorm &&
                                     format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
                          } );
        
        if ( iter != available_formats.cend( ))
        {
            return *iter;
        }
        else
        {
            return available_formats[0];
        }
    }
    
    const vk::PresentModeKHR swapchain::choose_present_mode( const std::vector<vk::PresentModeKHR>& available_present_modes ) const noexcept
    {
        const auto iter =
            std::find_if( available_present_modes.cbegin( ), available_present_modes.cend( ),
                          []( const vk::PresentModeKHR& present_mode )
                          {
                              return present_mode == vk::PresentModeKHR::eMailbox;
                          } );
        
        if ( iter != available_present_modes.cend( ))
        {
            return *iter;
        }
        else
        {
            return vk::PresentModeKHR::eFifo;
        }
    }
    
#undef max
#undef min

    const vk::Extent2D swapchain::choose_extent( const vk::SurfaceCapabilitiesKHR& capabilities ) const noexcept
    {
        if ( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max( ))
        {
            return capabilities.currentExtent;
        }
        else
        {
            const auto width = std::max( capabilities.minImageExtent.width,
                                         std::min( capabilities.maxImageExtent.width, width_ ));
            
            const auto height = std::max( capabilities.minImageExtent.height,
                                          std::min( capabilities.maxImageExtent.height, height_ ));
            
            return { width, height };
        }
    }
}
