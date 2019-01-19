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

#ifndef ENGINE_SWAPCHAIN_HPP
#define ENGINE_SWAPCHAIN_HPP

#include <vulkan/vulkan.hpp>

#include "../twe_core.hpp"

namespace twe::vulkan
{
    struct swapchain
    {
    private:
        struct support_details_type;
    
    public:
        struct create_info_type;
    
    public:
        TWE_API swapchain( ) = default;
        
        TWE_API swapchain( const create_info_type& create_info );
        
        TWE_API swapchain( const swapchain& rhs ) = delete;
        
        TWE_API swapchain( swapchain&& rhs );
        
        TWE_API ~swapchain( ) = default;
        
        TWE_API swapchain& operator=( const swapchain& rhs ) = delete;
        
        TWE_API swapchain& operator=( swapchain&& rhs );
        
        TWE_API void recreate( const create_info_type& create_info );
    
    private:
        template<class C>
        std::enable_if_t<std::is_same_v<C, vk::UniqueSwapchainKHR>, C> create_handle(
            const vk::Device device,
            const vk::SurfaceKHR surface,
            const vk::PresentModeKHR& present_mode,
            const vk::SurfaceCapabilitiesKHR& capabilities ) const noexcept
        {
            const auto create_info = vk::SwapchainCreateInfoKHR( )
                .setSurface( surface )
                .setPresentMode( present_mode )
                .setImageFormat( surface_format_.format )
                .setImageColorSpace( surface_format_.colorSpace )
                .setImageExtent( extent_ )
                .setImageArrayLayers( 1 )
                .setImageUsage( vk::ImageUsageFlagBits::eColorAttachment )
                .setImageSharingMode( vk::SharingMode::eExclusive )
                .setMinImageCount( image_count_ )
                .setQueueFamilyIndexCount( 0 )
                .setPQueueFamilyIndices( nullptr )
                .setPreTransform( capabilities.currentTransform )
                .setCompositeAlpha( vk::CompositeAlphaFlagBitsKHR::eOpaque )
                .setClipped( VK_TRUE );
            
            return device.createSwapchainKHRUnique( create_info );
        }
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, vk::UniqueImageView>, C> create_handle(
            const vk::Device device,
            const vk::Image& image ) const noexcept
        {
            const auto mapping = vk::ComponentMapping( )
                .setR( vk::ComponentSwizzle::eIdentity )
                .setG( vk::ComponentSwizzle::eIdentity )
                .setB( vk::ComponentSwizzle::eIdentity )
                .setA( vk::ComponentSwizzle::eIdentity );
            
            const auto subresources_range = vk::ImageSubresourceRange( )
                .setAspectMask( vk::ImageAspectFlagBits::eColor )
                .setBaseMipLevel( 0 )
                .setLevelCount( 1 )
                .setBaseArrayLayer( 0 )
                .setLayerCount( 1 );
            
            const auto create_info = vk::ImageViewCreateInfo( )
                .setFormat( surface_format_.format )
                .setComponents( mapping )
                .setSubresourceRange( subresources_range )
                .setImage( image )
                .setViewType( vk::ImageViewType::e2D );
            
            return device.createImageViewUnique( create_info );
        }
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, vk::UniqueFramebuffer>, C> create_handle(
            const vk::Device device,
            const vk::RenderPass render_pass,
            const vk::ImageView attachment ) const noexcept
        {
            const auto create_info = vk::FramebufferCreateInfo( )
                .setRenderPass( render_pass )
                .setWidth( extent_.width )
                .setHeight( extent_.height )
                .setAttachmentCount( 1 )
                .setPAttachments( &attachment )
                .setLayers( 1 );
            
            return device.createFramebufferUnique( create_info );
        }
        
        
        const vk::SurfaceFormatKHR choose_surface_format( const std::vector<vk::SurfaceFormatKHR>& available_formats ) const noexcept;
        
        const vk::PresentModeKHR choose_present_mode( const std::vector<vk::PresentModeKHR>& available_present_modes ) const noexcept;
        
        const vk::Extent2D choose_extent( const vk::SurfaceCapabilitiesKHR& capabilities ) const noexcept;
    
    private:
        std::uint32_t width_;
        std::uint32_t height_;
        
    public:
        std::uint32_t image_count_;
        
        vk::SurfaceFormatKHR surface_format_;
        vk::Extent2D extent_;
        
        vk::UniqueSwapchainKHR swapchain_;
        
        std::vector<vk::Image> images_;
        std::vector<vk::UniqueImageView> image_views_;
        std::vector<vk::UniqueFramebuffer> framebuffers_;
    
    public:
        struct create_info_type
        {
            create_info_type(
                vk::PhysicalDevice gpu = vk::PhysicalDevice( ),
                vk::Device device = vk::Device( ),
                vk::SurfaceKHR surface = vk::SurfaceKHR( ),
                vk::RenderPass render_pass = vk::RenderPass( ),
                std::uint32_t width = 0,
                std::uint32_t height = 0 )
                :
                gpu_( gpu ),
                device_( device ),
                surface_( surface ),
                render_pass_( render_pass ),
                width_( width ),
                height_( height )
            { }
            
            create_info_type& set_gpu( const vk::PhysicalDevice gpu )
            {
                gpu_ = gpu;
                return *this;
            }
            
            create_info_type& set_device( const vk::Device device )
            {
                device_ = device;
                return *this;
            }
            
            create_info_type& set_surface( const vk::SurfaceKHR surface )
            {
                surface_ = surface;
                return *this;
            }
            
            create_info_type& set_render_pass( const vk::RenderPass render_pass )
            {
                render_pass_ = render_pass;
                return *this;
            }
            
            create_info_type& set_width( const std::uint32_t width )
            {
                width_ = width;
                return *this;
            }
            
            create_info_type& set_height( const std::uint32_t height )
            {
                height_ = height;
                return *this;
            }
            
            vk::PhysicalDevice gpu_;
            vk::Device device_;
            vk::SurfaceKHR surface_;
            vk::RenderPass render_pass_;
            std::uint32_t width_;
            std::uint32_t height_;
        };
    
    private:
        struct support_details_type
        {
            support_details_type& set_capabilities( const vk::SurfaceCapabilitiesKHR& capabilities )
            {
                capabilities_ = capabilities;
                return *this;
            }
            
            support_details_type& set_formats( const std::vector<vk::SurfaceFormatKHR>& formats )
            {
                formats_ = formats;
                return *this;
            }
            
            support_details_type& set_present_modes( const std::vector<vk::PresentModeKHR>& present_modes )
            {
                present_modes_ = present_modes;
                return *this;
            }
            
            vk::SurfaceCapabilitiesKHR capabilities_;
            std::vector<vk::SurfaceFormatKHR> formats_;
            std::vector<vk::PresentModeKHR> present_modes_;
        };
    };
}

#endif //ENGINE_SWAPCHAIN_HPP
