/*!
 *  Copyright (C) 2018 BouwnLaw
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

#include <iostream>
#include <map>
#include <set>

#include <glm/glm.hpp>

#include "utilities/file_io.h"
#include "renderer.h"

VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback_function( VkDebugReportFlagsEXT flags,
                         VkDebugReportObjectTypeEXT objType,
                         uint64_t obj, size_t location,
                         int32_t code, const char* layerPrefix,
                         const char* msg, void* userData )
{
    std::cerr << "validation layer: " << msg << std::endl;

    return VK_FALSE;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDebugReportCallbackEXT ( VkInstance instance,
                                 const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                 const VkAllocationCallbacks* pAllocator,
                                 VkDebugReportCallbackEXT* pCallback )
{
    static auto func = ( PFN_vkCreateDebugReportCallbackEXT ) vkGetInstanceProcAddr ( instance, "vkCreateDebugReportCallbackEXT" );

    if ( func != nullptr )
    {
        return func ( instance, pCreateInfo, pAllocator, pCallback );
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
VKAPI_ATTR void VKAPI_CALL
vkDestroyDebugReportCallbackEXT ( VkInstance instance,
                                  VkDebugReportCallbackEXT callback,
                                  const VkAllocationCallbacks* pAllocator )
{
    static auto func = ( PFN_vkDestroyDebugReportCallbackEXT ) vkGetInstanceProcAddr ( instance, "vkDestroyDebugReportCallbackEXT" );

    if ( func != nullptr )
    {
        func ( instance, callback, pAllocator );
    }
}

namespace engine
{
    renderer::renderer( const engine::window& wnd, const std::string& app_name, uint32_t app_version )
        :
        window_width_( wnd.get_width() ),
        window_height_( wnd.get_height() )
    {
        const std::vector<const char*> glfw_extensions = wnd.get_required_extensions();
        const std::vector<const char*> validation_layers = { "VK_LAYER_LUNARG_standard_validation" };
        const std::vector<const char*> device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };


        if( !check_extensions_support( glfw_extensions ) )
        {
            throw std::runtime_error{ "Extensions requested, but not supported!" };
        }

        if( enable_validation_layers && !check_validation_layer_support( validation_layers ) )
        {
            throw std::runtime_error{ "Validation Layers requested, but not supported!" };
        }


        {   /// Create Vulkan Instance ///
            instance_handle_ = create_instance( glfw_extensions, validation_layers, app_name, app_version );

            if( !instance_handle_ )
                throw std::runtime_error{ "Failed to create Vulkan Instance!" };
        }   //////////////////////////////


        {   /// Create Vulkan Debug Callback ///
            debug_report_callback_handle_ = create_debug_report_callback();

            if( !debug_report_callback_handle_ )
                throw std::runtime_error{ "Failed to create Debug Report Callback!" };
        }   ////////////////////////////////////


        {   /// Create Surface ///
            surface_handle_ = create_surface( wnd );

            if( !surface_handle_ )
                throw std::runtime_error{ "Failed to create Surface!" };
        }   //////////////////////


        {   /// Choose Physical Device ///
            physical_device_handle_ = pick_physical_device( device_extensions );

            if( !physical_device_handle_ )
                std::runtime_error{ "Failed to find a valid GPU for Vulkan!" };
        }   //////////////////////////////


        {   /// Create Logical Device ///
            logical_device_handle_ = create_logical_device( validation_layers, device_extensions );

            if( !logical_device_handle_ )
                std::runtime_error{ "Failed to create Logical Device!" };
        }   /////////////////////////////

        auto queue_family_indices = find_queue_families( physical_device_handle_ );

        graphics_queue_ = logical_device_handle_.getQueue( static_cast<uint32_t>( queue_family_indices.graphics_family_ ), 0 );
        present_queue_ = logical_device_handle_.getQueue( static_cast<uint32_t>( queue_family_indices.present_family_ ), 0 );


        {   /// Create Swapchain ///
            const swapchain_support_details swapchain_support_details
            {
                physical_device_handle_.getSurfaceCapabilitiesKHR( surface_handle_ ),
                physical_device_handle_.getSurfaceFormatsKHR( surface_handle_ ),
                physical_device_handle_.getSurfacePresentModesKHR( surface_handle_ )
            };

            const auto surface_format = choose_swapchain_surface_format( swapchain_support_details.formats_ );
            const auto present_mode = choose_swapchain_present_mode( swapchain_support_details.present_modes_ );
            swapchain_image_extent_2d_ = choose_swapchain_extent_2d( swapchain_support_details.capabilities_ );

            uint32_t image_count = swapchain_support_details.capabilities_.minImageCount + 1;

            if( swapchain_support_details.capabilities_.maxImageCount > 0 && image_count > swapchain_support_details.capabilities_.maxImageCount )
                image_count = swapchain_support_details.capabilities_.maxImageCount;

            swapchain_handle_ = create_swapchain( swapchain_support_details, surface_format,
                                                  swapchain_image_extent_2d_, present_mode, image_count );
            swapchain_image_handles_ = logical_device_handle_.getSwapchainImagesKHR( swapchain_handle_ );
            swapchain_image_format_ = surface_format.format;

            if( !swapchain_handle_ )
                throw std::runtime_error{ "Failed to create Swapchain!" };

            for( const auto& image_handle : swapchain_image_handles_ )
            {
                if( !image_handle )
                    throw std::runtime_error{ "Failed to create Swapchain Image!" };
            }
        }   ////////////////////////

        {   /// Create Image Views ///
            swapchain_image_view_handles_ = create_swapchain_image_views( );

            for( const auto& image_view_handle : swapchain_image_view_handles_ )
            {
                if( !image_view_handle )
                    throw std::runtime_error{ "Failed to create Image View!" };
            }
        }   //////////////////////////


        {   /// Create Render Pass ///
            render_pass_handle_ = create_render_pass();

            if( !render_pass_handle_ )
                throw std::runtime_error{ "Failed to create Render Pass!" };
        }   //////////////////////////

        {   /// Create Framebuffers ///
            swapchain_framebuffer_handles_ = create_swapchain_framebuffers( );

            for( const auto& framebuffer_handle : swapchain_framebuffer_handles_ )
            {
                if( !framebuffer_handle )
                    throw std::runtime_error{ "Failed to create Framebuffer!" };
            }
        }   ///////////////////////////
    }
    renderer::~renderer( )
    {
        logical_device_handle_.destroyPipeline( graphics_pipeline_handle_ );
        logical_device_handle_.destroyPipelineLayout( graphics_pipeline_layout_handle_ );

        for( auto& framebuffer_handle : swapchain_framebuffer_handles_ )
        {
            logical_device_handle_.destroyFramebuffer( framebuffer_handle );
        }

        logical_device_handle_.destroyRenderPass( render_pass_handle_ );

        for( auto& image_view_handle : swapchain_image_view_handles_ )
        {
            logical_device_handle_.destroyImageView( image_view_handle );
        }

        logical_device_handle_.destroySwapchainKHR( swapchain_handle_ );
        logical_device_handle_.destroy( );

        instance_handle_.destroySurfaceKHR( surface_handle_ );
        instance_handle_.destroyDebugReportCallbackEXT( debug_report_callback_handle_ );
        instance_handle_.destroy( );
    }

    void renderer::setup_graphics_pipeline( const std::string& vert_shader_filepath,
                                            const std::string& frag_shader_filepath )
    {
        /// Create Vertex Shader Module ///
        const vk::ShaderModule vert_shader_handle = create_shader_module( utilities::read_from_binary_file( vert_shader_filepath ) );

        if( !vert_shader_handle )
            throw std::runtime_error{ "Failed to create Vertex Shader!" };
        ///////////////////////////////////

        /// Create Fragment Shader Module ///
        const vk::ShaderModule frag_shader_handle = create_shader_module( utilities::read_from_binary_file( frag_shader_filepath ) );

        if( !frag_shader_handle )
            throw std::runtime_error{ "Failed to create Fragment Shader!" };
        /////////////////////////////////////

        /// Create Pipeline Layout ///
        graphics_pipeline_layout_handle_ = create_graphics_pipeline_layout( );

        if( !graphics_pipeline_layout_handle_ )
            throw std::runtime_error{ "Failed to create Graphics Pipeline Layout!" };
        //////////////////////////////

        /// Create Pipeline ///
        graphics_pipeline_handle_ = create_graphics_pipeline( vert_shader_handle, frag_shader_handle );

        if( !graphics_pipeline_handle_ )
            throw std::runtime_error{ "Failed to create Graphics Pipeline!" };
        ///////////////////////

        logical_device_handle_.destroyShaderModule( vert_shader_handle );
        logical_device_handle_.destroyShaderModule( frag_shader_handle );
    }

    const vk::Instance renderer::create_instance( const std::vector<const char*>& extensions,
                                                  const std::vector<const char*>& validation_layers,
                                                  const std::string& app_name, uint32_t app_version ) const noexcept
    {
        const vk::ApplicationInfo application_info
        {
            app_name.c_str( ), app_version,
            "No Engine", VK_MAKE_VERSION( 0, 0, 1 ),
            VK_API_VERSION_1_1
        };

        const vk::InstanceCreateInfo create_info = ( enable_validation_layers )
                ? vk::InstanceCreateInfo{ { }, &application_info,
                    static_cast<uint32_t>( validation_layers.size() ), validation_layers.data(),
                    static_cast<uint32_t>( extensions.size() ), extensions.data() }
                : vk::InstanceCreateInfo{ { }, &application_info,
                    0, nullptr,
                    static_cast<uint32_t>( extensions.size() ), extensions.data() };

        return vk::createInstance( create_info );
    }
    const vk::DebugReportCallbackEXT renderer::create_debug_report_callback( ) const noexcept
    {
        const vk::DebugReportCallbackCreateInfoEXT create_info
        {
            vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::eError,
            debug_callback_function
        };

        return instance_handle_.createDebugReportCallbackEXT( create_info );
    }
    const vk::SurfaceKHR renderer::create_surface( const window& wnd ) const noexcept
    {
        auto [result, surface_handle] = wnd.create_surface( instance_handle_ );

        return ( result == VK_SUCCESS )
                    ? surface_handle
                    : nullptr;
    }
    const vk::PhysicalDevice renderer::pick_physical_device( const std::vector<const char*>& device_extensions ) const noexcept
    {
        std::vector<vk::PhysicalDevice> physical_devices = instance_handle_.enumeratePhysicalDevices( );
        std::multimap<int, vk::PhysicalDevice> candidates;

        if( !physical_devices.empty() )
        {
            for( const auto& device : physical_devices )
            {
                if( is_physical_device_suitable( device, device_extensions ) )
                {
                    int score = rate_physical_device( device );
                    candidates.insert( std::make_pair( score, device ) );
                }
            }

            return ( candidates.rbegin()->first > 0 )
                        ? candidates.rbegin()->second
                        : nullptr;
        }
        else
        {
            return nullptr;
        }
    }
    const vk::Device renderer::create_logical_device( const std::vector<const char*>& validation_layers,
                                                      const std::vector<const char*>& device_extensions ) const noexcept
    {
        auto queue_family_indices = find_queue_families( physical_device_handle_ );
        std::set<int> unique_queue_family;

        if( queue_family_indices.graphics_family_ >= 0 )
            unique_queue_family.insert( queue_family_indices.graphics_family_ );

        if( queue_family_indices.compute_family_ >= 0 )
            unique_queue_family.insert( queue_family_indices.compute_family_ );

        if( queue_family_indices.present_family_ >= 0 )
            unique_queue_family.insert( queue_family_indices.present_family_ );

        std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
        queue_create_infos.reserve( unique_queue_family.size() );

        float queue_priority = 1.0f;
        for( const auto& queue_family : unique_queue_family )
        {
            queue_create_infos.emplace_back( vk::DeviceQueueCreateInfo( { }, queue_family, 1, &queue_priority ) );
        }

        const auto features = physical_device_handle_.getFeatures( );
        const auto create_info = ( enable_validation_layers )
                ? vk::DeviceCreateInfo{ { },
                                        static_cast<uint32_t>( queue_create_infos.size() ), queue_create_infos.data(),
                                        static_cast<uint32_t>( validation_layers.size() ), validation_layers.data(),
                                        static_cast<uint32_t>( device_extensions.size() ), device_extensions.data(),
                                        &features }
                : vk::DeviceCreateInfo{ { },
                                        static_cast<uint32_t>( queue_create_infos.size() ), queue_create_infos.data(),
                                        0, nullptr,
                                        static_cast<uint32_t>( device_extensions.size() ), device_extensions.data(),
                                        &features };

        return physical_device_handle_.createDevice( create_info );
    }
    const vk::SwapchainKHR renderer::create_swapchain( const swapchain_support_details& swapchain_support_details,
                                                       const vk::SurfaceFormatKHR& surface_format, const vk::Extent2D& extent_2d,
                                                       const vk::PresentModeKHR& present_mode, const uint32_t image_count ) const noexcept
    {
        const auto indices = find_queue_families( physical_device_handle_ );
        const uint32_t queue_family_indices[] =
        {
            static_cast<uint32_t>( indices.graphics_family_ ),
            static_cast<uint32_t>( indices.present_family_ )
        };

        const auto create_info = ( indices.graphics_family_ != indices.present_family_ )
                ? vk::SwapchainCreateInfoKHR{
                    { }, surface_handle_, image_count, surface_format.format, surface_format.colorSpace,
                    extent_2d, 1, vk::ImageUsageFlagBits::eColorAttachment,
                    vk::SharingMode::eConcurrent, 2, queue_family_indices,
                    swapchain_support_details.capabilities_.currentTransform,
                    vk::CompositeAlphaFlagBitsKHR::eOpaque, present_mode, VK_TRUE, nullptr }
                : vk::SwapchainCreateInfoKHR{
                    { }, surface_handle_, image_count, surface_format.format, surface_format.colorSpace,
                    extent_2d, 1, vk::ImageUsageFlagBits::eColorAttachment,
                    vk::SharingMode::eExclusive, 0, nullptr,
                    swapchain_support_details.capabilities_.currentTransform,
                    vk::CompositeAlphaFlagBitsKHR::eOpaque, present_mode, VK_TRUE, nullptr };

        return logical_device_handle_.createSwapchainKHR( create_info );
    }
    const std::vector<vk::ImageView> renderer::create_swapchain_image_views( ) const noexcept
    {
        std::vector<vk::ImageView> image_view_handles( swapchain_image_handles_.size() );

        for( size_t i = 0; i < swapchain_image_handles_.size(); ++i )
        {
            vk::ImageViewCreateInfo create_info
            {
                { }, swapchain_image_handles_[i],
                vk::ImageViewType::e2D,
                swapchain_image_format_,
                { vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
                  vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity },
                { vk::ImageAspectFlagBits::eColor,
                  0, 1, 0, 1 }
            };

            image_view_handles[i] = logical_device_handle_.createImageView( create_info );
        }

        return image_view_handles;
    }
    const vk::ShaderModule renderer::create_shader_module( const std::string& shader_code ) const noexcept
    {
        const vk::ShaderModuleCreateInfo create_info
        {
            { }, shader_code.size(),
            reinterpret_cast<const uint32_t*>( shader_code.data() )
        };

        return logical_device_handle_.createShaderModule( create_info );
    }
    const vk::RenderPass renderer::create_render_pass( ) const noexcept
    {
        const vk::AttachmentDescription colour_attachment_description
        {
            { }, swapchain_image_format_, vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR
        };
        const vk::AttachmentReference colour_attachment_reference
        {
            0, vk::ImageLayout::eColorAttachmentOptimal
        };
        const vk::SubpassDescription subpass_description
        {
            { }, vk::PipelineBindPoint::eGraphics,
            0, nullptr, 1, &colour_attachment_reference,
            nullptr, nullptr, 0, nullptr
        };
        const vk::RenderPassCreateInfo create_info
        {
                { },
                1, &colour_attachment_description,
                1, &subpass_description,
                0, nullptr
        };

        return logical_device_handle_.createRenderPass( create_info );
    }
    const std::vector<vk::Framebuffer> renderer::create_swapchain_framebuffers( ) const noexcept
    {
        std::vector<vk::Framebuffer> framebuffer_handles( swapchain_image_view_handles_.size( ) );

        for( size_t i = 0; i < framebuffer_handles.size(); ++i )
        {
            const vk::FramebufferCreateInfo create_info
                    {
                            { }, render_pass_handle_,
                            1, &swapchain_image_view_handles_[i],
                            swapchain_image_extent_2d_.width,
                            swapchain_image_extent_2d_.height,
                            1
                    };

            framebuffer_handles[i] = logical_device_handle_.createFramebuffer( create_info );
        }

        return framebuffer_handles;
    }
    const vk::PipelineLayout renderer::create_graphics_pipeline_layout( ) const noexcept
    {
        const vk::PipelineLayoutCreateInfo create_info{ { }, 0, nullptr, 0, nullptr };

        return logical_device_handle_.createPipelineLayout( create_info );
    }
    const vk::Pipeline renderer::create_graphics_pipeline( const vk::ShaderModule& vert_shader_handle,
                                                           const vk::ShaderModule& frag_shader_handle ) const noexcept
    {
        const vk::PipelineShaderStageCreateInfo shader_stages[] =
        {
            { { }, vk::ShaderStageFlagBits::eVertex, vert_shader_handle, "main" },
            { { }, vk::ShaderStageFlagBits::eFragment, frag_shader_handle, "main" }
        };
        const vk::PipelineVertexInputStateCreateInfo vertex_input_state_create_info
        {
            { },
            0, nullptr,
            0, nullptr
        };
        const vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_create_info
        {
            { },
            vk::PrimitiveTopology::eTriangleList,
            VK_FALSE
        };
        const vk::Viewport viewport
        {
            0.0f, 0.0f,
            static_cast<float>( swapchain_image_extent_2d_.width ),
            static_cast<float>( swapchain_image_extent_2d_.height ),
            0.0f, 1.0f
        };
        const vk::Rect2D scissors
        {
            { 0, 0 },
            swapchain_image_extent_2d_
        };
        const vk::PipelineViewportStateCreateInfo viewport_state_create_info
        {
            { },
            1, &viewport,
            1, &scissors
        };
        const vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info
        {
            { }, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise,
            VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f
        };
        const vk::PipelineMultisampleStateCreateInfo multisample_state_create_info
        {
            { }, vk::SampleCountFlagBits::e1, VK_FALSE,
            1.0f, nullptr, VK_FALSE, VK_FALSE
        };
        /// no depth buffer as of right now.
        const vk::PipelineColorBlendAttachmentState colour_blend_attachment_state
        {
            VK_FALSE,
            vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
            vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
        };
        const vk::PipelineColorBlendStateCreateInfo colour_blend_state_create_info
        {
            { }, VK_FALSE, vk::LogicOp::eCopy,
            1, &colour_blend_attachment_state,
            { 0.0f, 0.0f, 0.0f, 0.0f }
        };
        const vk::PipelineDynamicStateCreateInfo dynamic_state_create_info
        {
            { },
            0, nullptr
        };
        const vk::GraphicsPipelineCreateInfo create_info
        {
            { },
            2, shader_stages,
            &vertex_input_state_create_info,
            &input_assembly_state_create_info,
            nullptr,
            &viewport_state_create_info,
            &rasterization_state_create_info,
            &multisample_state_create_info,
            nullptr,
            &colour_blend_state_create_info,
            &dynamic_state_create_info,
            graphics_pipeline_layout_handle_,
            render_pass_handle_, 0,
            { }, 0
        };

        return logical_device_handle_.createGraphicsPipeline( nullptr, create_info );
    }

    bool renderer::check_extensions_support( const std::vector<const char*>& extensions )
    {
        for( const auto& extension : extensions )
        {
            bool is_supported = false;

            for( const auto& extension_property : vk::enumerateInstanceExtensionProperties( ) )
            {
                if( strcmp( extension, extension_property.extensionName ) == 0 )
                {
                    is_supported = true;
                    break;
                }
            }

            if( !is_supported )
            {
                return false;
            }
        }

        return true;
    }
    bool renderer::check_physical_device_extension_support( const vk::PhysicalDevice& physical_device_handle,
                                                            const std::vector<const char*>& device_extensions ) const noexcept
    {
        std::set<std::string> required_extensions( device_extensions.begin(), device_extensions.end() );

        for( const auto& extension : physical_device_handle.enumerateDeviceExtensionProperties( ) )
        {
            required_extensions.erase( extension.extensionName );
        }

        return required_extensions.empty();
    }
    bool renderer::check_validation_layer_support( const std::vector<const char*>& validation_layers )
    {
        for( const auto& layer_name : validation_layers )
        {
            bool is_supported = false;

            for( const auto& layer_properties : vk::enumerateInstanceLayerProperties( ) )
            {
                if( strcmp( layer_name, layer_properties.layerName ) == 0 )
                {
                    is_supported = true;
                    break;
                }
            }

            if( !is_supported )
            {
                return false;
            }
        }

        return true;
    }

    int renderer::rate_physical_device( const vk::PhysicalDevice& physical_device_handle ) const noexcept
    {
        auto physical_device_properties = physical_device_handle.getProperties( );
        auto physical_device_features = physical_device_handle.getFeatures( );

        int score = 0;

        if( physical_device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu )
        {
            score += 1000;
        }
        else if( physical_device_properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu )
        {
            score += 500;
        }
        else
        {
            return 0;
        }

        if( !physical_device_features.geometryShader )
        {
            return 0;
        }

        score += physical_device_properties.limits.maxImageDimension2D;

        return score;
    }

    bool renderer::is_physical_device_suitable( const vk::PhysicalDevice& physical_device_handle,
                                                const std::vector<const char*>& device_extensions ) const noexcept
    {
        return ( !find_queue_families( physical_device_handle ).is_complete() )
                    ? false
                    : ( !check_physical_device_extension_support( physical_device_handle, device_extensions ) )
                           ? false
                           : ( is_swapchain_adequate( physical_device_handle ) );
    }
    bool renderer::is_swapchain_adequate( const vk::PhysicalDevice& physical_device_handle ) const noexcept
    {
        const swapchain_support_details swapchain_support_details
        {
            physical_device_handle.getSurfaceCapabilitiesKHR( surface_handle_ ),
            physical_device_handle.getSurfaceFormatsKHR( surface_handle_ ),
            physical_device_handle.getSurfacePresentModesKHR( surface_handle_ )
        };

        // TODO: make better. (look at specs).

        return !swapchain_support_details.formats_.empty() && !swapchain_support_details.present_modes_.empty( );
    }

    renderer::queue_family_indices renderer::find_queue_families( const vk::PhysicalDevice& physical_device_handle ) const noexcept
    {
        queue_family_indices indices;

        int i = 0;
        for( const auto& property : physical_device_handle.getQueueFamilyProperties( ) )
        {
            if( property.queueCount > 0 )
            {
                if( property.queueFlags & vk::QueueFlagBits::eGraphics )
                {
                    indices.graphics_family_ = i;
                }
                if( property.queueFlags & vk::QueueFlagBits::eCompute )
                {
                    indices.compute_family_ = i;
                }
                if( physical_device_handle.getSurfaceSupportKHR( i, surface_handle_ ) )
                {
                    indices.present_family_ = i;
                }

                if( indices.is_complete() )
                {
                    break;
                }

                ++i;
            }
        }

        return indices;
    }

    vk::SurfaceFormatKHR renderer::choose_swapchain_surface_format( const std::vector<vk::SurfaceFormatKHR>& available_formats ) const noexcept
    {
        if( available_formats.size() == 1 && available_formats[0].format == vk::Format::eUndefined )
        {
            return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
        }

        for( const auto& available_format : available_formats )
        {
            if( available_format.format == vk::Format::eR8G8B8A8Unorm && available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear )
            {
                return available_format;
            }
        }

        return available_formats[0];
    }
    vk::PresentModeKHR renderer::choose_swapchain_present_mode( const std::vector<vk::PresentModeKHR>& available_present_modes ) const noexcept
    {
        vk::PresentModeKHR best_mode = vk::PresentModeKHR::eFifo;

        for( const auto& available_present_mode : available_present_modes )
        {
            if( available_present_mode == vk::PresentModeKHR::eMailbox )
            {
                return available_present_mode;
            }
            else if( available_present_mode == vk::PresentModeKHR::eImmediate )
            {
                best_mode = available_present_mode;
            }
        }

        return best_mode;
    }
    vk::Extent2D renderer::choose_swapchain_extent_2d( const vk::SurfaceCapabilitiesKHR& capabilities ) const noexcept
    {
        if( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() )
        {
            return capabilities.currentExtent;
        }
        else
        {
            vk::Extent2D actual_extent( window_width_, window_height_ );

            actual_extent.width = std::clamp( actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
            actual_extent.height = std::clamp( actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );

            return actual_extent;
        }
    }
}
