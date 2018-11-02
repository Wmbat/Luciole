/*!
 *  Copyright (C) 2018 Wmbat
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
#include <set>
#include <map>

#include "utilities/file_io.h"
#include "renderer.h"

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

#ifndef NDEBUG
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
#endif

namespace engine
{
    renderer::renderer( const engine::window& wnd, const std::string& app_name, uint32_t app_version )
        //:
        //window_width_( wnd.get_width() ),
        //window_height_( wnd.get_height() )
    {
        uint32_t supported_api_version;
        if( vkEnumerateInstanceVersion( &supported_api_version ) != VK_SUCCESS )
        {
            std::runtime_error{ "Vulkan Not Installed!" };
        }

        if( supported_api_version != VK_API_VERSION_1_1 )
        {
            std::runtime_error{ "Vulkan version 1.1+ not supported! Check vulkan version and if necessary, install latest version 1.1 and up." };
        }

        const std::vector<const char*> instance_extensions = { };//wnd.get_required_extensions();
        const std::vector<const char*> validation_layers = { "VK_LAYER_LUNARG_standard_validation" };
        const std::vector<const char*> device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        if( !check_extensions_support( instance_extensions ) )
        {
            throw std::runtime_error{ "Extensions requested, but not supported!" };
        }

        if( !enable_debug_layers && !check_validation_layer_support( validation_layers ) )
        {
            throw std::runtime_error{ "Validation Layers requested, but not supported!" };
        }


        instance_ = create_instance( instance_extensions, validation_layers, app_name, app_version );

        if( !instance_ )
            throw std::runtime_error{ "Failed to create Vulkan Instance! Check Drivers." };


#ifndef NDEBUG
        debug_report_callback_ = create_debug_report_callback( instance_ );

        if( !debug_report_callback_ )
            throw std::runtime_error{ "Failed to create Debug Report Callback!" };
#endif


        surface_ = create_surface( wnd, instance_ );

        if( !surface_ )
            throw std::runtime_error{ "Failed to create Surface!" };


        physical_device_ = pick_physical_device( device_extensions, instance_ );

        if( !physical_device_ )
            std::runtime_error{ "Failed to find a valid GPU for Vulkan!" };


        logical_device_ = create_logical_device( validation_layers, device_extensions, physical_device_ );

        if( !logical_device_ )
            std::runtime_error{ "Failed to create Logical Device!" };



        image_available_semaphores_.resize( MAX_FRAMES_IN_FLIGHT );

        for( auto& semaphore : image_available_semaphores_ )
        {
            semaphore = create_semaphore( logical_device_ );

            if( !semaphore )
                std::runtime_error{ "Failed to create Image Available Semaphore!" };
        }

        render_finished_semaphores_.resize( MAX_FRAMES_IN_FLIGHT );

        for( auto& semaphore : render_finished_semaphores_ )
        {
            semaphore = create_semaphore( logical_device_ );

            if( !semaphore )
                std::runtime_error{ "Failed to create Render Finished Semaphore!" };
        }

        frame_in_flight_fences_.resize( MAX_FRAMES_IN_FLIGHT );

        for( auto& fence : frame_in_flight_fences_ )
        {
            fence = create_fence( logical_device_ );

            if( !fence )
                std::runtime_error{ "Failed to create In-Fligh Fence!" };
        }

        auto queue_family_indices = find_queue_families( physical_device_ );

        graphics_queue_ = logical_device_.getQueue( static_cast<uint32_t>( queue_family_indices.graphics_family_ ), 0 );
        present_queue_ = logical_device_.getQueue( static_cast<uint32_t>( queue_family_indices.present_family_ ), 0 );

        {
            const swapchain_support_details swapchain_support_details
            {
                physical_device_.getSurfaceCapabilitiesKHR( surface_ ),
                physical_device_.getSurfaceFormatsKHR( surface_ ),
                physical_device_.getSurfacePresentModesKHR( surface_ )
            };

            const auto surface_format = choose_swapchain_surface_format( swapchain_support_details.formats_ );
            const auto present_mode = choose_swapchain_present_mode( swapchain_support_details.present_modes_ );
            swapchain_image_extent_2d_ = choose_swapchain_extent_2d( swapchain_support_details.capabilities_ );

            uint32_t image_count = swapchain_support_details.capabilities_.minImageCount + 1;

            if( swapchain_support_details.capabilities_.maxImageCount > 0 && image_count > swapchain_support_details.capabilities_.maxImageCount )
                image_count = swapchain_support_details.capabilities_.maxImageCount;

            swapchain_ = create_swapchain( surface_, logical_device_, physical_device_,
                                                  swapchain_support_details, surface_format,
                                                  swapchain_image_extent_2d_, present_mode, image_count );
            swapchain_images_ = logical_device_.getSwapchainImagesKHR( swapchain_ );
            swapchain_image_format_ = surface_format.format;

            if( !swapchain_ )
                throw std::runtime_error{ "Failed to create Swapchain!" };

            for( const auto& image_handle : swapchain_images_ )
            {
                if( !image_handle )
                    throw std::runtime_error{ "Failed to create Swapchain Image!" };
            }
        }

        swapchain_image_views_ = create_swapchain_image_views( logical_device_,
                swapchain_image_format_,
                swapchain_images_,
                static_cast<uint32_t>( swapchain_images_.size() ) );

        for( const auto& image_view_handle : swapchain_image_views_ )
        {
            if( !image_view_handle )
                throw std::runtime_error{ "Failed to create Image View!" };
        }



        render_pass_ = create_render_pass( logical_device_, swapchain_image_format_ );

        if( !render_pass_ )
            throw std::runtime_error{ "Failed to create Render Pass!" };



        command_pool_ = create_command_pool( logical_device_, queue_family_indices );

        if( !command_pool_ )
            throw std::runtime_error{ "Failed to create Command Pool!" };


        command_buffers_ = allocate_command_buffers( logical_device_,
                command_pool_,
                static_cast<uint32_t>( swapchain_image_views_.size() ) );

        for( const auto& command_buffer_handle : command_buffers_ )
        {
            if( !command_buffer_handle )
                throw std::runtime_error{ "Failed to allocate Command Buffer" };
        }


        swapchain_framebuffers_ = create_swapchain_framebuffers( logical_device_,
                render_pass_,
                swapchain_image_extent_2d_,
                swapchain_image_views_,
                static_cast<uint32_t>( swapchain_image_views_.size() ) );

        for( const auto& framebuffer_handle : swapchain_framebuffers_ )
        {
            if( !framebuffer_handle )
                throw std::runtime_error{ "Failed to create Framebuffer!" };
        }
    }
    renderer::renderer( renderer&& renderer ) noexcept
    {
        *this = std::move( renderer );
    }
    renderer::~renderer( )
    {
        present_queue_.waitIdle( );

        logical_device_.destroyPipeline( graphics_pipeline_ );
        logical_device_.destroyPipelineLayout( graphics_pipeline_layout_ );

        for( auto& framebuffer_handle : swapchain_framebuffers_ )
        {
            logical_device_.destroyFramebuffer( framebuffer_handle );
        }

        logical_device_.freeCommandBuffers( command_pool_, static_cast<uint32_t>( command_buffers_.size() ), command_buffers_.data() );

        logical_device_.destroyCommandPool( command_pool_ );

        logical_device_.destroyRenderPass( render_pass_ );

        for( auto& image_view_handle : swapchain_image_views_ )
        {
            logical_device_.destroyImageView( image_view_handle );
        }

        logical_device_.destroySwapchainKHR( swapchain_ );

        for( auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i )
        {
            logical_device_.destroySemaphore( image_available_semaphores_[i] );
            logical_device_.destroySemaphore( render_finished_semaphores_[i] );
            logical_device_.destroyFence( frame_in_flight_fences_[i] );
        }

        logical_device_.destroy( );

        instance_.destroySurfaceKHR( surface_ );
#ifndef NDEBUG
        instance_.destroyDebugReportCallbackEXT( debug_report_callback_ );
#endif
        instance_.destroy( );
    }

    renderer &renderer::operator=( renderer &&renderer ) noexcept
    {
        if( this != &renderer )
        {
            instance_ = renderer.instance_;
            renderer.instance_ = nullptr;

#ifndef NDEBUG
            debug_report_callback_ = renderer.debug_report_callback_;
            renderer.debug_report_callback_ = nullptr;
#endif

            surface_ = renderer.surface_;
            renderer.surface_ = nullptr;

            physical_device_ = renderer.physical_device_;
            renderer.physical_device_ = nullptr;

            logical_device_ = renderer.logical_device_;
            renderer.logical_device_ = nullptr;

            graphics_queue_ = renderer.graphics_queue_;
            renderer.graphics_queue_ = nullptr;

            present_queue_ = renderer.present_queue_;
            renderer.present_queue_ = nullptr;

            image_available_semaphores_ = renderer.image_available_semaphores_;
            renderer.image_available_semaphores_ = { };

            render_finished_semaphores_ = renderer.render_finished_semaphores_;
            renderer.render_finished_semaphores_ = { };

            frame_in_flight_fences_ = renderer.frame_in_flight_fences_;
            renderer.frame_in_flight_fences_ = { };

            swapchain_ = renderer.swapchain_;
            renderer.swapchain_ = nullptr;

            swapchain_image_format_ = renderer.swapchain_image_format_;
            renderer.swapchain_image_format_ = vk::Format( );

            swapchain_image_extent_2d_ = renderer.swapchain_image_extent_2d_;
            renderer.swapchain_image_extent_2d_ = vk::Extent2D( );

            swapchain_images_ = renderer.swapchain_images_;
            renderer.swapchain_images_ = { };

            swapchain_image_views_ = renderer.swapchain_image_views_;
            renderer.swapchain_image_views_ = { };

            swapchain_framebuffers_ = renderer.swapchain_framebuffers_;
            renderer.swapchain_framebuffers_ = { };

            render_pass_ = renderer.render_pass_;
            renderer.render_pass_ = nullptr;

            command_pool_ = renderer.command_pool_;
            renderer.command_pool_ = nullptr;

            command_buffers_ = renderer.command_buffers_;
            renderer.command_buffers_ = { };

            graphics_pipeline_layout_ = renderer.graphics_pipeline_layout_;
            renderer.graphics_pipeline_layout_ = nullptr;

            graphics_pipeline_ = renderer.graphics_pipeline_;
            renderer.graphics_pipeline_ = nullptr;

            window_width_ = renderer.window_width_;
            renderer.window_width_ = 0;

            window_height_ = renderer.window_height_;
            renderer.window_height_ = 0;
        }

        return *this;
    }

    void renderer::setup_graphics_pipeline( const std::string& vert_shader_filepath,
                                            const std::string& frag_shader_filepath )
    {
        const vk::ShaderModule vert_shader_handle = create_shader_module( logical_device_, utilities::read_from_binary_file( vert_shader_filepath ) );

        if( !vert_shader_handle )
            throw std::runtime_error{ "Failed to create Vertex Shader!" };


        const vk::ShaderModule frag_shader_handle = create_shader_module( logical_device_, utilities::read_from_binary_file( frag_shader_filepath ) );

        if( !frag_shader_handle )
            throw std::runtime_error{ "Failed to create Fragment Shader!" };


        graphics_pipeline_layout_ = create_graphics_pipeline_layout( logical_device_ );

        if( !graphics_pipeline_layout_ )
            throw std::runtime_error{ "Failed to create Graphics Pipeline Layout!" };


        graphics_pipeline_ = create_graphics_pipeline( logical_device_,
                swapchain_image_extent_2d_,
                graphics_pipeline_layout_,
                render_pass_,
                vert_shader_handle,
                frag_shader_handle );

        if( !graphics_pipeline_ )
            throw std::runtime_error{ "Failed to create Graphics Pipeline!" };

        logical_device_.destroyShaderModule( vert_shader_handle );
        logical_device_.destroyShaderModule( frag_shader_handle );
    }

    void renderer::handle_resize( const std::string& vert_shader_filepath,
                                  const std::string& frag_shader_filepath,
                                  std::uint32_t width, std::uint32_t height )
    {
        window_width_ = width;
        window_height_ = height;
    }

    void renderer::record_command_buffers( ) const noexcept
    {
        for( auto i = 0; i < command_buffers_.size(); ++i )
        {
            const vk::CommandBufferBeginInfo begin_info
            {
                vk::CommandBufferUsageFlagBits::eSimultaneousUse, nullptr
            };

            command_buffers_[i].begin( begin_info );
            {
                const std::array<float, 4> clear_colour = { 0.0f, 0.0f, 0.0f, 1.0f };

                const vk::ClearColorValue clear_colour_value{ clear_colour };
                const vk::ClearValue clear_value{ clear_colour_value };

                const vk::RenderPassBeginInfo render_pass_begin_info
                {
                    render_pass_, swapchain_framebuffers_[i],
                    vk::Rect2D{ { 0, 0 }, swapchain_image_extent_2d_ },
                    1, &clear_value
                };

                command_buffers_[i].beginRenderPass( render_pass_begin_info, vk::SubpassContents::eInline );
                {
                    command_buffers_[i].bindPipeline( vk::PipelineBindPoint::eGraphics, graphics_pipeline_ );
                    command_buffers_[i].draw( 3, 1, 0, 0 );
                }
                command_buffers_[i].endRenderPass( );
            }
            command_buffers_[i].end( );
        }
    }
    void renderer::draw_frame( const std::string& vert_shader_filepath,
                               const std::string& frag_shader_filepath )
    {
        logical_device_.waitForFences( frame_in_flight_fences_[current_frame_], VK_TRUE, std::numeric_limits<uint64_t>::max( ) );

        const auto result = logical_device_.acquireNextImageKHR( swapchain_, std::numeric_limits<uint64_t>::max(),
                image_available_semaphores_[current_frame_], nullptr );

        if( result.result == vk::Result::eErrorOutOfDateKHR )
        {
            recreate_swapchain( vert_shader_filepath, frag_shader_filepath );
            record_command_buffers();
        }
        else if( result.result != vk::Result::eSuccess && result.result != vk::Result::eSuboptimalKHR )
        {
            throw std::runtime_error{ "Failed to acquire swapchain image!" };
        }

        const vk::Semaphore wait_semaphores[] = { image_available_semaphores_[current_frame_] };
        const vk::Semaphore signal_semaphores[] = { render_finished_semaphores_[current_frame_] };
        const vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        const vk::SubmitInfo submit_info
        {
            1, wait_semaphores,
            wait_stages,
            1,
            &command_buffers_[result.value],
            1, signal_semaphores
        };

        logical_device_.resetFences( frame_in_flight_fences_[current_frame_] );

        if( graphics_queue_.submit( 1, &submit_info, frame_in_flight_fences_[current_frame_] ) != vk::Result::eSuccess )
        {
            throw std::runtime_error{ "Failed to submit draw command to buffer" };
        }

        /*
        const vk::SwapchainKHR swapchains[] = { swapchain_ };
        const vk::PresentInfoKHR present_info
        {
            1, signal_semaphores,
            1, swapchains,
            &( result.value ),
            nullptr
        };


        const auto present_result = present_queue_.presentKHR( present_info );

        if( present_result == vk::Result::eErrorOutOfDateKHR || present_result == vk::Result::eSuboptimalKHR )
        {
            recreate_swapchain( vert_shader_filepath, frag_shader_filepath );
            record_command_buffers();
        }
        else if( present_result != vk::Result::eSuccess )
        {
            throw std::runtime_error{ "Failed to present swapchain Image!" };
        }
         */

        const VkSwapchainKHR test_swapchains[] = { swapchain_ };
        const VkSemaphore  test_semaphores[] = { signal_semaphores[current_frame_] };
        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = test_semaphores;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = test_swapchains;
        present_info.pImageIndices = &( result.value );

        const auto test_result = vkQueuePresentKHR( present_queue_, &present_info );

        if( test_result == VK_ERROR_OUT_OF_DATE_KHR || test_result == VK_SUBOPTIMAL_KHR )
        {
            recreate_swapchain( vert_shader_filepath, frag_shader_filepath );
            record_command_buffers();
        }
        else if( test_result != VK_SUCCESS )
        {
            throw std::runtime_error{ "Failed to present swapchain image!" };
        }

        current_frame_ = ( current_frame_ + 1 ) & MAX_FRAMES_IN_FLIGHT;
    }


    void renderer::recreate_swapchain( const std::string& vert_shader_filepath,
                                       const std::string& frag_shader_filepath )
    {
        logical_device_.waitIdle( );

        cleanup_swapchain();

        {
            const swapchain_support_details swapchain_support_details
            {
                physical_device_.getSurfaceCapabilitiesKHR( surface_ ),
                physical_device_.getSurfaceFormatsKHR( surface_ ),
                physical_device_.getSurfacePresentModesKHR( surface_ )
            };

            const auto surface_format = choose_swapchain_surface_format( swapchain_support_details.formats_ );
            const auto present_mode = choose_swapchain_present_mode( swapchain_support_details.present_modes_ );
            swapchain_image_extent_2d_ = choose_swapchain_extent_2d( swapchain_support_details.capabilities_ );

            uint32_t image_count = swapchain_support_details.capabilities_.minImageCount + 1;

            if( swapchain_support_details.capabilities_.maxImageCount > 0 && image_count > swapchain_support_details.capabilities_.maxImageCount )
                image_count = swapchain_support_details.capabilities_.maxImageCount;

            swapchain_ = create_swapchain( surface_, logical_device_, physical_device_,
                                           swapchain_support_details, surface_format,
                                           swapchain_image_extent_2d_, present_mode, image_count );
            swapchain_images_ = logical_device_.getSwapchainImagesKHR( swapchain_ );
            swapchain_image_format_ = surface_format.format;

            if( !swapchain_ )
                throw std::runtime_error{ "Failed to create Swapchain!" };

            for( const auto& image_handle : swapchain_images_ )
            {
                if( !image_handle )
                    throw std::runtime_error{ "Failed to create Swapchain Image!" };
            }
        }

        swapchain_image_views_ = create_swapchain_image_views( logical_device_,
                                                               swapchain_image_format_,
                                                               swapchain_images_,
                                                               static_cast<uint32_t>( swapchain_images_.size() ) );

        for( const auto& image_view_handle : swapchain_image_views_ )
        {
            if( !image_view_handle )
                throw std::runtime_error{ "Failed to create Image View!" };
        }


        render_pass_ = create_render_pass( logical_device_, swapchain_image_format_ );

        if( !render_pass_ )
            throw std::runtime_error{ "Failed to create Render Pass!" };


        setup_graphics_pipeline( vert_shader_filepath, frag_shader_filepath );


        swapchain_framebuffers_ = create_swapchain_framebuffers( logical_device_,
                                                                 render_pass_,
                                                                 swapchain_image_extent_2d_,
                                                                 swapchain_image_views_,
                                                                 static_cast<uint32_t>( swapchain_image_views_.size() ) );

        for( const auto& framebuffer_handle : swapchain_framebuffers_ )
        {
            if( !framebuffer_handle )
                throw std::runtime_error{ "Failed to create Framebuffer!" };
        }

        command_buffers_ = allocate_command_buffers( logical_device_,
                                                     command_pool_,
                                                     static_cast<uint32_t>( swapchain_image_views_.size() ) );

        for( const auto& command_buffer_handle : command_buffers_ )
        {/*!
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
            if( !command_buffer_handle )
                throw std::runtime_error{ "Failed to allocate Command Buffer" };
        }
    }
    void renderer::cleanup_swapchain( ) noexcept
    {
        for( auto& framebuffer : swapchain_framebuffers_ )
        {
            logical_device_.destroyFramebuffer( framebuffer );
        }

        logical_device_.freeCommandBuffers( command_pool_, static_cast<uint32_t>( command_buffers_.size() ), command_buffers_.data() );

        logical_device_.destroyPipeline( graphics_pipeline_ );
        logical_device_.destroyPipelineLayout( graphics_pipeline_layout_ );
        logical_device_.destroyRenderPass( render_pass_ );

        for( auto& image_view : swapchain_image_views_ )
        {
            logical_device_.destroyImageView( image_view );
        }

        logical_device_.destroySwapchainKHR( swapchain_ );
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

        if( enable_debug_layers )
        {
            const vk::InstanceCreateInfo create_info
            {
                { }, &application_info,
                static_cast<uint32_t>( validation_layers.size() ), validation_layers.data(),
                static_cast<uint32_t>( extensions.size() ), extensions.data()
            };

            return vk::createInstance( create_info );
        }
        else
        {
            const vk::InstanceCreateInfo create_info
            {
                { }, &application_info,
                0, nullptr,
                static_cast<uint32_t>( extensions.size() ), extensions.data()
            };

            return vk::createInstance( create_info );
        }

    }

#ifndef NDEBUG
    const vk::DebugReportCallbackEXT renderer::create_debug_report_callback( const vk::Instance& instance ) const noexcept
    {
        const vk::DebugReportCallbackCreateInfoEXT create_info
        {
            vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::eError,
            debug_callback_function
        };

        return instance.createDebugReportCallbackEXT( create_info );
    }
#endif
    const vk::SurfaceKHR renderer::create_surface( const window& wnd, const vk::Instance& instance ) const noexcept
    {
        return nullptr;

        /*
        auto surface = //wnd.create_surface( instance );

        return ( surface.result_ == VK_SUCCESS )
                    ? surface.value_
                    : nullptr;
                    */
    }
    const vk::PhysicalDevice renderer::pick_physical_device( const std::vector<const char*>& device_extensions,
                                                             const vk::Instance& instance ) const noexcept
    {
        std::vector<vk::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices( );
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
                                                      const std::vector<const char*>& device_extensions,
                                                      const vk::PhysicalDevice& physical_device ) const noexcept
    {
        auto queue_family_indices = find_queue_families( physical_device_);
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

        const auto features = physical_device.getFeatures( );
        const auto create_info = ( enable_debug_layers )
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

        return physical_device.createDevice( create_info );
    }
    const vk::Semaphore renderer::create_semaphore( const vk::Device& logical_device ) const noexcept
    {
        const vk::SemaphoreCreateInfo create_info
        {
            vk::SemaphoreCreateFlags( )
        };

        return logical_device.createSemaphore( create_info );
    }
    const vk::Fence renderer::create_fence( const vk::Device& logical_device ) const noexcept
    {
        const vk::FenceCreateInfo create_info
        {
            vk::FenceCreateFlagBits::eSignaled
        };

        return logical_device.createFence( create_info );
    }
    const vk::SwapchainKHR renderer::create_swapchain( const vk::SurfaceKHR& surface,
                                                       const vk::Device& logical_device,
                                                       const vk::PhysicalDevice& physical_device,
                                                       const swapchain_support_details& swapchain_support_details,
                                                       const vk::SurfaceFormatKHR& surface_format, const vk::Extent2D& extent_2d,
                                                       const vk::PresentModeKHR& present_mode,
                                                       uint32_t image_count ) const noexcept
    {
        const auto indices = find_queue_families( physical_device );
        const uint32_t queue_family_indices[] =
        {
            static_cast<uint32_t>( indices.graphics_family_ ),
            static_cast<uint32_t>( indices.present_family_ )
        };

        const auto create_info = ( indices.graphics_family_ != indices.present_family_ )
                ? vk::SwapchainCreateInfoKHR{
                    { }, surface_, image_count, surface_format.format, surface_format.colorSpace,
                    extent_2d, 1, vk::ImageUsageFlagBits::eColorAttachment,
                    vk::SharingMode::eConcurrent, 2, queue_family_indices,
                    swapchain_support_details.capabilities_.currentTransform,
                    vk::CompositeAlphaFlagBitsKHR::eOpaque, present_mode, VK_TRUE, nullptr }
                : vk::SwapchainCreateInfoKHR{
                    { }, surface_, image_count, surface_format.format, surface_format.colorSpace,
                    extent_2d, 1, vk::ImageUsageFlagBits::eColorAttachment,
                    vk::SharingMode::eExclusive, 0, nullptr,
                    swapchain_support_details.capabilities_.currentTransform,
                    vk::CompositeAlphaFlagBitsKHR::eOpaque, present_mode, VK_TRUE, nullptr };

        return logical_device.createSwapchainKHR( create_info );
    }
    const std::vector<vk::ImageView> renderer::create_swapchain_image_views( const vk::Device& logical_device,
                                                                             const vk::Format& swapchain_image_format,
                                                                             const std::vector<vk::Image>& swapchain_images,
                                                                             uint32_t image_view_count ) const noexcept
    {
        std::vector<vk::ImageView> image_view_handles( image_view_count );

        for( size_t i = 0; i < image_view_count; ++i )
        {
            vk::ImageViewCreateInfo create_info
            {
                { }, swapchain_images[i],
                vk::ImageViewType::e2D,
                swapchain_image_format,
                { vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
                  vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity },
                { vk::ImageAspectFlagBits::eColor,
                  0, 1, 0, 1 }
            };

            image_view_handles[i] = logical_device.createImageView( create_info );
        }

        return image_view_handles;
    }
    const vk::ShaderModule renderer::create_shader_module( const vk::Device& logical_device,
                                                           const std::string& shader_code ) const noexcept
    {
        const vk::ShaderModuleCreateInfo create_info
        {
            { }, shader_code.size(),
            reinterpret_cast<const uint32_t*>( shader_code.data() )
        };

        return logical_device.createShaderModule( create_info );
    }
    const vk::RenderPass renderer::create_render_pass( const vk::Device& logical_device,
                                                       const vk::Format& swapchain_image_format ) const noexcept
    {
        const vk::AttachmentDescription colour_attachment_description
        {
            { }, swapchain_image_format, vk::SampleCountFlagBits::e1,
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
        const vk::SubpassDependency subpass_dependency
        {
            VK_SUBPASS_EXTERNAL, 0,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            { },
            vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite
        };
        const vk::RenderPassCreateInfo create_info
        {
            { },
            1, &colour_attachment_description,
            1, &subpass_description,
            1, &subpass_dependency
        };

        return logical_device.createRenderPass( create_info );
    }
    const std::vector<vk::Framebuffer> renderer::create_swapchain_framebuffers( const vk::Device& logical_device,
                                                                                const vk::RenderPass& render_pass,
                                                                                const vk::Extent2D& swapchain_image_extent_2d,
                                                                                const std::vector<vk::ImageView>& swapchain_image_views,
                                                                                uint32_t framebuffer_count ) const noexcept
    {
        std::vector<vk::Framebuffer> framebuffer_handles( swapchain_image_views_.size( ) );

        for( size_t i = 0; i < framebuffer_handles.size(); ++i )
        {
            const vk::FramebufferCreateInfo create_info
            {
                { }, render_pass,
                1, &swapchain_image_views[i],
                swapchain_image_extent_2d.width,
                swapchain_image_extent_2d.height,
                1
            };

            framebuffer_handles[i] = logical_device.createFramebuffer( create_info );
        }

        return framebuffer_handles;
    }
    const vk::CommandPool renderer::create_command_pool( const vk::Device& logical_device,
                                                         const queue_family_indices& queue_family_indices ) const noexcept
    {
        const vk::CommandPoolCreateInfo create_info
        {
            { }, static_cast<uint32_t>( queue_family_indices.graphics_family_ )
        };

        return logical_device.createCommandPool( create_info );
    }
    const std::vector<vk::CommandBuffer> renderer::allocate_command_buffers( const vk::Device& logical_device,
                                                                             const vk::CommandPool& command_pool,
                                                                             uint32_t buffer_count ) const noexcept
    {
        const vk::CommandBufferAllocateInfo allocate_info
        {
            command_pool,
            vk::CommandBufferLevel::ePrimary,
            buffer_count
        };

        return logical_device.allocateCommandBuffers( allocate_info );
    }
    const vk::PipelineLayout renderer::create_graphics_pipeline_layout( const vk::Device& logical_device ) const noexcept
    {
        const vk::PipelineLayoutCreateInfo create_info
        {
            { },
            0, nullptr,     /// Descriptor sets.
            0, nullptr      /// Push constants.
        };

        return logical_device.createPipelineLayout( create_info );
    }
    const vk::Pipeline renderer::create_graphics_pipeline( const vk::Device& logical_device,
                                                           const vk::Extent2D& swapchain_image_extent_2d,
                                                           const vk::PipelineLayout& graphics_pipeline_layout,
                                                           const vk::RenderPass& render_pass,
                                                           const vk::ShaderModule& vert_shader_handle,
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
            static_cast<float>( swapchain_image_extent_2d.width ),
            static_cast<float>( swapchain_image_extent_2d.height ),
            0.0f, 1.0f
        };
        const vk::Rect2D scissors
        {
            { 0, 0 },
            swapchain_image_extent_2d
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
            0, nullptr                      /// No Dynamic States yet.
        };
        const vk::GraphicsPipelineCreateInfo create_info
        {
            { },
            2, shader_stages,
            &vertex_input_state_create_info,
            &input_assembly_state_create_info,
            nullptr,                        /// No tesselation yet.
            &viewport_state_create_info,
            &rasterization_state_create_info,
            &multisample_state_create_info,
            nullptr,                        /// No Depth buffer yet.
            &colour_blend_state_create_info,
            nullptr,                        /// No dynamic states yet.
            graphics_pipeline_layout,
            render_pass, 0,
            { }, 0
        };

        return logical_device.createGraphicsPipeline( nullptr, create_info );
    }

    bool renderer::check_extensions_support( const std::vector<const char*>& extensions ) const noexcept
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
    bool renderer::check_physical_device_extension_support( const vk::PhysicalDevice& physical_device,
                                                            const std::vector<const char*>& device_extensions ) const noexcept
    {
        std::set<std::string> required_extensions( device_extensions.begin(), device_extensions.end() );

        for( const auto& extension : physical_device.enumerateDeviceExtensionProperties( ) )
        {
            required_extensions.erase( extension.extensionName );
        }

        return required_extensions.empty();
    }
    bool renderer::check_validation_layer_support( const std::vector<const char*>& validation_layers ) const noexcept
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

    int renderer::rate_physical_device( const vk::PhysicalDevice& physical_device ) const noexcept
    {
        auto physical_device_properties = physical_device.getProperties( );
        auto physical_device_features = physical_device.getFeatures( );

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

    bool renderer::is_physical_device_suitable( const vk::PhysicalDevice& physical_device,
                                                const std::vector<const char*>& device_extensions ) const noexcept
    {
        return ( !find_queue_families( physical_device ).is_complete() )
                    ? false
                    : ( !check_physical_device_extension_support( physical_device, device_extensions ) )
                           ? false
                           : ( is_swapchain_adequate( physical_device ) );
    }
    bool renderer::is_swapchain_adequate( const vk::PhysicalDevice& physical_device ) const noexcept
    {
        const swapchain_support_details swapchain_support_details
        {
            physical_device.getSurfaceCapabilitiesKHR( surface_ ),
            physical_device.getSurfaceFormatsKHR( surface_ ),
            physical_device.getSurfacePresentModesKHR( surface_ )
        };

        // TODO: make better. (look at specs).

        return !swapchain_support_details.formats_.empty() && !swapchain_support_details.present_modes_.empty( );
    }

    renderer::queue_family_indices renderer::find_queue_families( const vk::PhysicalDevice& physical_device ) const noexcept
    {
        queue_family_indices indices;

        int i = 0;
        for( const auto& property : physical_device.getQueueFamilyProperties( ) )
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
                if( physical_device.getSurfaceSupportKHR( i, surface_ ) )
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