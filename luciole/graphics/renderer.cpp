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

#include <wmbats_bazaar/file_io.hpp>
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>

#include "renderer.hpp"

renderer::renderer( p_context_t  p_context )
    :
    p_context_( p_context.value_ )
{
    auto const capabilities = p_context_->get_surface_capabilities();
    auto const format = pick_swapchain_format();
    
    swapchain_image_format_ = format.format;
    swapchain_extent_ = pick_swapchain_extent( capabilities );
    
    std::uint32_t image_count = capabilities.minImageCount + 1;
    if ( capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount )
    {
        image_count = capabilities.maxImageCount;
    }
    
    swapchain_images_.reserve( image_count );
    swapchain_image_views_.reserve( image_count );
    swapchain_framebuffers_.reserve( image_count );
    
    swapchain_ = vk_check(
        vk_swapchain_t( create_swapchain( capabilities, format ) ),
        error_msg_t( "Failed to create Swapchain!" ) );
    
    swapchain_images_ = vk_check_array(
        p_context_->get_swapchain_images( vk_swapchain_t( swapchain_ ), count32_t( image_count ) ),
        error_msg_t( "Failed to create Swapchain Images!" ) );
    
    swapchain_image_views_ = vk_check_array(
        create_image_views( count32_t( image_count ) ),
        error_msg_t( "Failed to create Swapchain Image Views!" )
        );
    
    render_pass_ = vk_check(
        vk_render_pass_t( create_render_pass() ),
        error_msg_t( "Failed to create Render Pass!" ) );
}
renderer::renderer( renderer&& rhs )
{
    *this = std::move( rhs );
}
renderer::~renderer( )
{
    if ( render_pass_ != VK_NULL_HANDLE )
    {
        p_context_->destroy_render_pass( vk_render_pass_t( render_pass_ ) );
        render_pass_ = VK_NULL_HANDLE;
    }
    
    for( auto& image_view : swapchain_image_views_ )
    {
        if ( image_view != VK_NULL_HANDLE )
        {
            p_context_->destroy_image_view( vk_image_view_t( image_view ) );
            image_view = VK_NULL_HANDLE;
        }
    }
    
    if ( swapchain_ != VK_NULL_HANDLE )
    {
        p_context_->destroy_swapchain( swapchain_ );
        swapchain_ = VK_NULL_HANDLE;
    }
}

renderer& renderer::operator=( renderer&& rhs )
{
    if ( this != &rhs )
    {
        std::swap( p_context_, rhs.p_context_ );
        
        std::swap( swapchain_, rhs.swapchain_ );
        std::swap( swapchain_images_, rhs.swapchain_images_ );
        std::swap( swapchain_extent_, rhs.swapchain_extent_ );
        std::swap( swapchain_image_format_, rhs.swapchain_image_format_ );
        std::swap( swapchain_image_views_, rhs.swapchain_image_views_ );
    }
    
    return *this;
}

VkSwapchainKHR renderer::create_swapchain( VkSurfaceCapabilitiesKHR const& capabilities, VkSurfaceFormatKHR const& format ) const
{
    auto const present_mode = pick_swapchain_present_mode( );
    
    auto create_info = p_context_->swapchain_create_info( );
    create_info.minImageCount = static_cast<std::uint32_t>( swapchain_images_.capacity( ) );
    create_info.imageFormat = format.format;
    create_info.imageColorSpace = format.colorSpace;
    create_info.imageExtent = swapchain_extent_;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = nullptr;
    
    return p_context_->create_swapchain( vk_swapchain_create_info_t( create_info ) );
}

std::vector<VkImageView> renderer::create_image_views( count32_t image_count ) const
{
    std::vector<VkImageView> image_views;
    image_views.reserve( image_count.value_ );
    
    for ( auto i = 0; i < image_count.value_; ++i )
    {
        VkImageView handle = VK_NULL_HANDLE;
    
        VkImageViewCreateInfo create_info
         {
             .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
             .pNext = nullptr,
             .flags = { },
             .image = swapchain_images_[i],
             .viewType = VK_IMAGE_VIEW_TYPE_2D,
             .format = swapchain_image_format_,
             .components = VkComponentMapping
             {
                 .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                 .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                 .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                 .a = VK_COMPONENT_SWIZZLE_IDENTITY
             },
             .subresourceRange = VkImageSubresourceRange
             {
                 .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                 .baseMipLevel = 0,
                 .levelCount = 1,
                 .baseArrayLayer = 0,
                 .layerCount = 1
             }
         };
    
        handle = p_context_->create_image_view( vk_image_view_create_info_t( create_info ) );
    
        if ( handle == VK_NULL_HANDLE )
        {
            return { };
        }
        
        image_views.emplace_back( handle );
    }
    
    return image_views;
}

VkRenderPass renderer::create_render_pass( ) const
{
    VkAttachmentDescription colour_attachment
    {
        .format = swapchain_image_format_,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };
    
    VkAttachmentReference colour_attachment_reference
    {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    
    VkSubpassDescription subpass_description
    {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colour_attachment_reference,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr
    };
    
    VkRenderPassCreateInfo create_info
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colour_attachment,
        .subpassCount = 1,
        .pSubpasses = &subpass_description,
        .dependencyCount = 0,
        .pDependencies = nullptr
    };
    
    return p_context_->create_render_pass( vk_render_pass_create_info_t( create_info ) );
}

VkShaderModule renderer::create_shader_module( shader_filepath_t filepath )
{


    // auto const resources = TBuilt; 
}

VkSurfaceFormatKHR renderer::pick_swapchain_format( ) const
{
    auto const formats = p_context_->get_surface_format( );
    
    for ( auto const& format : formats )
    {
        if ( format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
        {
            return format;
        }
    }
    
    return formats[0];
}

VkPresentModeKHR renderer::pick_swapchain_present_mode( ) const
{
    auto const present_modes = p_context_->get_present_modes( );
    
    for( auto const& present_mode : present_modes )
    {
        if ( present_mode == VK_PRESENT_MODE_MAILBOX_KHR )
        {
            return present_mode;
        }
    }
    
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D renderer::pick_swapchain_extent( VkSurfaceCapabilitiesKHR const& capabilities ) const
{
    if ( capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max( ) )
    {
        return capabilities.currentExtent;
    }
    else
    {
        auto actual_extent = p_context_->get_window_extent();
        
        actual_extent.width = std::clamp( actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
        actual_extent.height = std::clamp( actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );
        
        return actual_extent;
    }
}

