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

#include "renderer.hpp"

renderer::renderer( p_context_t p_context )
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

    default_graphics_pipeline_layout_ = vk_check(
        vk_pipeline_layout_t( create_default_pipeline_layout( ) ),
        error_msg_t( "Failed to create default Pipeline Layout!" ) );

    default_graphics_pipeline_ = vk_check(
        vk_pipeline_t( create_default_pipeline( 
            shader_filepath_t( "resources/shaders/default_vert.spv" ), 
            shader_filepath_t( "resources/shaders/default_frag.spv" ) ) ),
        error_msg_t( "Failed to create default Pipeline!" ) );

    auto x = p_context_->create_command_buffers( VK_QUEUE_GRAPHICS_BIT, count32_t( image_count ) );

    swapchain_framebuffers_ = vk_check_array( create_framebuffers( count32_t( image_count ) ), error_msg_t( "Failed to create Framebuffers!" ) );
    render_command_buffers_ = vk_check_array( x, error_msg_t( "Failed to create render command buffers!" ) );

    for( int i = 0; i < MAX_FRAMES_IN_FLIGHT_; ++i )
    {
        image_available_semaphore_[i] = vk_check( vk_semaphore_t( create_semaphore( ) ), error_msg_t( "Failed to create image available semaphore!" ) );
        render_finished_semaphore_[i] = vk_check( vk_semaphore_t( create_semaphore( ) ), error_msg_t( "Failed to create render finished semaphore!" ) );
        in_flight_fences[i] = vk_check( vk_fence_t( create_fence( ) ), error_msg_t( "Failed to create in-flight fence!" ) );
    }

    record_command_buffers( );
}
renderer::renderer( renderer&& rhs )
{
    *this = std::move( rhs );
}
renderer::~renderer( )
{
    for( int i = 0; i < MAX_FRAMES_IN_FLIGHT_; ++i )
    {
        if ( image_available_semaphore_[i] != VK_NULL_HANDLE )
        {
            p_context_->destroy_semaphore( vk_semaphore_t( image_available_semaphore_[i] ) );
            image_available_semaphore_[i] = VK_NULL_HANDLE;
        }

        if ( render_finished_semaphore_[i] != VK_NULL_HANDLE )
        {
            p_context_->destroy_semaphore( vk_semaphore_t( render_finished_semaphore_[i] ) );
            render_finished_semaphore_[i] = VK_NULL_HANDLE;
        }

        if ( in_flight_fences[i] != VK_NULL_HANDLE )
        {
            p_context_->destroy_fence( vk_fence_t( in_flight_fences[i] ) );
            in_flight_fences[i] = VK_NULL_HANDLE;
        }
    }

    for( auto& framebuffer : swapchain_framebuffers_ )
    {
        if ( framebuffer != VK_NULL_HANDLE )
        {
            p_context_->destroy_framebuffer( vk_framebuffer_t( framebuffer ) );
            framebuffer = VK_NULL_HANDLE;
        }
    }

    if ( default_graphics_pipeline_ != VK_NULL_HANDLE )
    {
        p_context_->destroy_pipeline( vk_pipeline_t( default_graphics_pipeline_ ) );
        default_graphics_pipeline_ = VK_NULL_HANDLE;
    }

    if ( default_graphics_pipeline_layout_ != VK_NULL_HANDLE )
    {
        p_context_->destroy_pipeline_layout( vk_pipeline_layout_t( default_graphics_pipeline_layout_ ) );
        default_graphics_pipeline_layout_ = VK_NULL_HANDLE;
    }

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
        std::swap( swapchain_, rhs.swapchain_ );
        std::swap( swapchain_images_, rhs.swapchain_images_ );
        std::swap( swapchain_extent_, rhs.swapchain_extent_ );
        std::swap( swapchain_image_format_, rhs.swapchain_image_format_ );
        std::swap( swapchain_image_views_, rhs.swapchain_image_views_ );
        std::swap( render_pass_, rhs.render_pass_ );
        std::swap( default_graphics_pipeline_, rhs.default_graphics_pipeline_ );
        std::swap( default_graphics_pipeline_layout_, rhs.default_graphics_pipeline_layout_ );
        std::swap( swapchain_framebuffers_, rhs.swapchain_framebuffers_ );
        std::swap( render_command_buffers_, rhs.render_command_buffers_ );
        
        
        for( int i = 0; i < MAX_FRAMES_IN_FLIGHT_; ++i )
        {
            image_available_semaphore_[i] = rhs.image_available_semaphore_[i];
            rhs.image_available_semaphore_[i] = VK_NULL_HANDLE;

            render_finished_semaphore_[i] = rhs.render_finished_semaphore_[i];
            rhs.render_finished_semaphore_[i] = VK_NULL_HANDLE;

            in_flight_fences[i] = rhs.in_flight_fences[i];
            rhs.in_flight_fences[i] = VK_NULL_HANDLE;
        }

        p_context_ = rhs.p_context_;
        rhs.p_context_ = nullptr;
    }
    
    return *this;
}

void renderer::draw_frame( )
{
    p_context_->reset_fence( vk_fence_t( in_flight_fences[current_frame] ) );

    std::uint32_t image_index = 0;
    vkAcquireNextImageKHR( p_context_->get( ), swapchain_, std::numeric_limits<std::uint64_t>::max( ), image_available_semaphore_[current_frame], VK_NULL_HANDLE, &image_index );

    VkSemaphore wait_semaphores[] = { image_available_semaphore_[current_frame] };
    VkSemaphore signal_semaphores[] = { render_finished_semaphore_[current_frame] };
    VkSwapchainKHR swapchains[] = { swapchain_ };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo const submit_info 
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = sizeof( wait_semaphores ) / sizeof( VkSemaphore ),
        .pWaitSemaphores = wait_semaphores,
        .pWaitDstStageMask = wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &render_command_buffers_[image_index],
        .signalSemaphoreCount = sizeof( signal_semaphores ) / sizeof( VkSemaphore ),
        .pSignalSemaphores = signal_semaphores
    };

    p_context_->submit_queue( VK_QUEUE_GRAPHICS_BIT, vk_submit_info_t( submit_info ), vk_fence_t( in_flight_fences[current_frame] ) );

    p_context_->wait_for_fence( vk_fence_t( in_flight_fences[current_frame] ) );


    VkPresentInfoKHR const present_info 
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = sizeof( signal_semaphores ) / sizeof( VkSemaphore ),
        .pWaitSemaphores = signal_semaphores,
        .swapchainCount = sizeof( swapchains ) / sizeof( VkSwapchainKHR ),
        .pSwapchains = swapchains,
        .pImageIndices = &image_index,
        .pResults = nullptr
    };

    p_context_->present_queue( VK_QUEUE_GRAPHICS_BIT, vk_present_info_t( present_info ) );

    current_frame = ( current_frame + 1 ) % MAX_FRAMES_IN_FLIGHT_;
}

void renderer::record_command_buffers( )
{
    for( size_t i = 0; i < render_command_buffers_.size( ); ++i )
    {
        VkCommandBufferBeginInfo const buffer_begin_info 
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };

        if ( vkBeginCommandBuffer( render_command_buffers_[i], &buffer_begin_info ) != VK_SUCCESS )
        {
            core_error( "Failed to begin recording command buffer!" );
        }

        VkClearValue const clear_colour = { 0.0f, 0.0f, 1.0f, 1.0f };

        VkRenderPassBeginInfo const pass_begin_info 
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = render_pass_,
            .framebuffer = swapchain_framebuffers_[i],
            .renderArea = VkRect2D 
            {
                .offset = {0, 0},
                .extent = swapchain_extent_
            },
            .clearValueCount = 1,
            .pClearValues = &clear_colour
        };

        vkCmdBeginRenderPass( render_command_buffers_[i], &pass_begin_info, VK_SUBPASS_CONTENTS_INLINE );

        vkCmdBindPipeline( render_command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, default_graphics_pipeline_ );

        vkCmdDraw( render_command_buffers_[i], 3, 1, 0, 0 );

        vkCmdEndRenderPass( render_command_buffers_[i] );

        if ( vkEndCommandBuffer( render_command_buffers_[i] ) != VK_SUCCESS )
        {
            core_error( "Failed to record command buffer!" );
        }
    }
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
    
    for ( uint32_t i = 0; i < image_count.value_; ++i )
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
    VkAttachmentDescription const colour_attachment
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
    
    VkAttachmentReference const colour_attachment_reference
    {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    
    VkSubpassDescription const subpass_description
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

    VkSubpassDependency const dependency 
    {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0
    };
    
    VkRenderPassCreateInfo const create_info
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colour_attachment,
        .subpassCount = 1,
        .pSubpasses = &subpass_description,
        .dependencyCount = 1,
        .pDependencies = &dependency
    };
    
    return p_context_->create_render_pass( vk_render_pass_create_info_t( create_info ) );
}

VkShaderModule renderer::create_shader_module( shader_filepath_t filepath ) const
{
    auto const spirv_code = bzr::read_from_binary_file( filepath.value_ );

    VkShaderModuleCreateInfo const create_info 
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = { },
        .codeSize = static_cast<std::uint32_t>( spirv_code.size( ) ),
        .pCode = reinterpret_cast<const std::uint32_t*>( spirv_code.data( ) )
    };

    return p_context_->create_shader_module( vk_shader_module_create_info_t( create_info ) );
}

VkPipelineLayout renderer::create_default_pipeline_layout( ) const
{
    VkPipelineLayoutCreateInfo const create_info 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    return p_context_->create_pipeline_layout( vk_pipeline_layout_create_info_t( create_info ) );
}

VkPipeline renderer::create_default_pipeline( shader_filepath_t vert_filepath, shader_filepath_t frag_filepath ) const
{
    auto const vert_shader = create_shader_module( vert_filepath );
    auto const frag_shader = create_shader_module( frag_filepath );

    VkPipelineShaderStageCreateInfo vert_shader_stage_create_info 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vert_shader,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo frag_shader_stage_create_info
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = frag_shader,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo shader_stage_create_infos[] = 
    { 
        vert_shader_stage_create_info,
        frag_shader_stage_create_info
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = { },
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    VkViewport viewport 
    {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>( swapchain_extent_.width ),
        .height = static_cast<float>( swapchain_extent_.height ),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor
    {
        .offset = { 0, 0 },
        .extent = swapchain_extent_
    };

    VkPipelineViewportStateCreateInfo viewport_state_create_info 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };

    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo multisample_state_create_info
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };

    VkPipelineColorBlendAttachmentState colour_blend_attachment_state
    {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo colour_blend_state_create_info
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colour_blend_attachment_state,
        .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
    };

    VkDynamicState dynamic_states[] = 
    {
        
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_create_info
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = sizeof ( dynamic_states ) / sizeof ( VkDynamicState ),
        .pDynamicStates = dynamic_states
    };

    VkGraphicsPipelineCreateInfo create_info 
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shader_stage_create_infos,
        .pVertexInputState = &vertex_input_state_create_info,
        .pInputAssemblyState = &input_assembly_state_create_info,
        .pTessellationState = nullptr,
        .pViewportState = &viewport_state_create_info,
        .pRasterizationState = &rasterization_state_create_info,
        .pMultisampleState = &multisample_state_create_info,
        .pColorBlendState = &colour_blend_state_create_info,
        .pDynamicState = &dynamic_state_create_info,
        .layout = default_graphics_pipeline_layout_,
        .renderPass = render_pass_,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0
    };

    auto const handle = p_context_->create_pipeline( vk_graphics_pipeline_create_info_t( create_info ) );

    p_context_->destroy_shader_module( vk_shader_module_t( frag_shader ) );
    p_context_->destroy_shader_module( vk_shader_module_t( vert_shader ) );

    return handle;
}

VkSemaphore renderer::create_semaphore( ) const
{
    VkSemaphoreCreateInfo const create_info 
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
    };
    
    return p_context_->create_semaphore( vk_semaphore_create_info_t( create_info ) );
}

VkFence renderer::create_fence( ) const noexcept
{
    VkFenceCreateInfo const create_info 
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    return p_context_->create_fence( vk_fence_create_info_t( create_info ) );
}

std::vector<VkFramebuffer> renderer::create_framebuffers( count32_t count ) const noexcept
{
    std::vector<VkFramebuffer> handles;
    handles.reserve( count.value_ );

    for( std::uint32_t i = 0; i < count.value_; ++i )
    {
        VkImageView attachments[] = {
            swapchain_image_views_[i]
        };

        VkFramebufferCreateInfo const create_info
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .flags = 0,
            .renderPass = render_pass_,
            .attachmentCount = sizeof( attachments ) / sizeof( VkImageView ),
            .pAttachments = attachments,
            .width = swapchain_extent_.width,
            .height = swapchain_extent_.height,
            .layers = 1
        };

        auto const handle = p_context_->create_framebuffer( vk_framebuffer_create_info_t( create_info ) );
        if ( handle == VK_NULL_HANDLE )
        {
            return { };
        }

        handles.emplace_back( handle );
    }

    return handles;
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

