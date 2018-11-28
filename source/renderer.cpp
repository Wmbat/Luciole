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
#include <renderer.h>


#include "utilities/file_io.h"
#include "utilities/basic_error.h"
#include "renderer.h"
#include "log.h"

static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

namespace TWE
{
    VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback_function( VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj, size_t location,
        int32_t code, const char* layerPrefix,
        const char* msg, void* userData )
    {
        if( flags & VK_DEBUG_REPORT_WARNING_BIT_EXT )
        {
            core_warn( "Validation Layers -> {0}.", msg );
        }
        else if( flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT )
        {
            core_warn( "Validation Layers -> {0}.", msg );
        }
        else if( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT )
        {
            core_error( "Validation Layers -> {0}.", msg );
        }
        
        return VK_FALSE;
    }
    VKAPI_ATTR VkResult VKAPI_CALL vk_create_debug_callback( VkInstance instance,
        const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugReportCallbackEXT* pCallback )
    {
        static auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
            vkGetInstanceProcAddr ( instance, "vkCreateDebugReportCallbackEXT" ) );
        
        if ( func != nullptr )
        {
            return func ( instance, pCreateInfo, pAllocator, pCallback );
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
    VKAPI_ATTR void VKAPI_CALL vk_destroy_debug_callback( VkInstance instance,
        VkDebugReportCallbackEXT callback,
        const VkAllocationCallbacks* pAllocator )
    {
        static auto func = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
            vkGetInstanceProcAddr ( instance, "vkDestroyDebugReportCallbackEXT" ) );
        
        if ( func != nullptr )
        {
            func ( instance, callback, pAllocator );
        }
    }
    
    
    renderer::renderer( const window& wnd, const std::string& app_name, uint32_t app_version )
        :
        window_width_( wnd.get_width() ),
        window_height_( wnd.get_height() )
    {
        try
        {
            set_up();
            
            core_info( "Using Vulkan for rendering." );
            
            vk_context_.instance_ = check_vk_return_type_result(
                create_instance( app_name, app_version ), "create_instance( )" );
            core_info( "Vulkan -> Instance created." );
    
            if constexpr( enable_debug_layers )
            {
                vk_context_.debug_report_ = check_vk_return_type_result(
                    create_debug_report( ), "create_debug_report( )" );
                core_info( "Vulkan -> Debug Report Callback created." );
            }
    
            vk_context_.surface_ = check_vk_return_type_result(
                create_surface( wnd ), "create_surface( )" );
            core_info( "Vulkan -> Surface created." );
    
            vk_context_.gpu_ = pick_physical_device( );
            core_info( "Vulkan -> Physical Device picked." );   // TODO: print Device info.
    
            vk_context_.device_ = check_vk_return_type_result( create_device( ), "create_device( )" );
            core_info( "Vulkan -> Device created." );
    
            /* Get the graphics queue and the present queue. */
            const auto queue_family_indices = find_queue_family_indices( vk_context_.surface_, vk_context_.gpu_ );
            vkGetDeviceQueue( vk_context_.device_, queue_family_indices.graphic_family_.value(), 0, &vk_context_.graphics_queue_ );
            vkGetDeviceQueue( vk_context_.device_, queue_family_indices.present_family_.value(), 0, &vk_context_.present_queue_ );
    
            core_info( "Vulkan -> Physical Device Graphics Queue. ID: {0:d}.", queue_family_indices.graphic_family_.value() );
            core_info( "Vulkan -> Physical Device Present Queue. ID: {0:d}.", queue_family_indices.present_family_.value() );
            core_info( "Vulkan -> Physical Device Compute Queue. ID: {0:d}.", queue_family_indices.compute_family_.value() );
    
            vk_context_.image_available_semaphores_.resize( MAX_FRAMES_IN_FLIGHT );
            for( auto& semaphore : vk_context_.image_available_semaphores_ )
            {
                semaphore = check_vk_return_type_result( create_semaphore(), "create_semaphore( )" );
            }
            core_info( "Vulkan -> Image Available Semaphores created" );
            
            vk_context_.render_finished_semaphores_.resize( MAX_FRAMES_IN_FLIGHT );
            for( auto& semaphore : vk_context_.render_finished_semaphores_ )
            {
                semaphore = check_vk_return_type_result( create_semaphore(), "create_semaphore( )" );
            }
            core_info( "Vulkan -> Render Finished Semaphore." );
            
            vk_context_.in_flight_fences_.resize( MAX_FRAMES_IN_FLIGHT );
            for( auto& fence : vk_context_.in_flight_fences_ )
            {
                fence = check_vk_return_type_result( create_fence(), "create_fence( )" );
            }
            core_info( "Vulkan -> In flight fences created." );
            
            vk_context_.command_pool_ = check_vk_return_type_result(
                create_command_pool( queue_family_indices.graphic_family_.value() ),
                "create_command_pool( )" );
            core_info( "Vulkan -> Command Pool created" );
    
            const auto swapchain_support_details = query_swapchain_support( vk_context_.surface_, vk_context_.gpu_ );
            const auto present_mode = choose_swapchain_present_mode( swapchain_support_details.present_modes_ );
            const auto surface_format = choose_swapchain_surface_format( swapchain_support_details.formats_ );
            const auto extent = choose_swapchain_extent( swapchain_support_details.capabilities_ );
    
            vk_context_.surface_format_ = surface_format;
            vk_context_.swapchain_extent_ = extent;
    
            uint32_t image_count = swapchain_support_details.capabilities_.minImageCount + 1;
            if( swapchain_support_details.capabilities_.maxImageCount > 0 &&
                image_count > swapchain_support_details.capabilities_.maxImageCount )
            {
                image_count = swapchain_support_details.capabilities_.maxImageCount;
            }
    
    
            vk_context_.swapchain_ = check_vk_return_type_result(
                create_swapchain( queue_family_indices, present_mode, swapchain_support_details.capabilities_,
                                  image_count ), "create_swapchain( )" );
            core_info( "Vulkan -> Swapchain created." );
    
            vkGetSwapchainImagesKHR( vk_context_.device_, vk_context_.swapchain_, &image_count, nullptr );
            vk_context_.swapchain_image_.resize( image_count );
            vkGetSwapchainImagesKHR( vk_context_.device_, vk_context_.swapchain_, &image_count, vk_context_.swapchain_image_.data( ) );
            core_info( "Vulkan -> Swapchain Images created. Count: {0:d}.", image_count );
    
            vk_context_.swapchain_image_views_.resize( image_count );
            for( auto i = 0; i < image_count; ++i )
            {
                vk_context_.swapchain_image_views_[i] = check_vk_return_type_result(
                    create_image_view( vk_context_.swapchain_image_[i] ), "create_image_view( )" );
            }
            core_info( "Vulkan -> Swapchain Image Views created. Count: {0:d}.", image_count );
    
    
            vk_context_.command_buffers_ = check_vk_return_type_result(
                create_command_buffers( image_count ), "create_command_buffers( )" );
            core_info( "Vulkan -> Command Buffers created. Count: {0:d}.", image_count );
            
            vk_context_.render_pass_ = check_vk_return_type_result(
                create_render_pass( ), "create_render_pass( )" );
            core_info( "Vulkan -> Render Pass created." );
    
            vk_context_.swapchain_framebuffers_.resize( image_count );
            for( auto i = 0; i < image_count; ++i )
            {
                vk_context_.swapchain_framebuffers_[i] = check_vk_return_type_result(
                    create_framebuffer( vk_context_.swapchain_image_views_[i] ), "create_framebuffer( )" );
            }
            core_info( "Vulkan -> Swapchain Framebuffers created. Count: {0:d}.", image_count );
        }
        catch( const basic_error& e )
        {
            core_error( e.what() );
        }
        catch( const vk_error& e )
        {
            core_error( e.what() );
        }
    }
    renderer::renderer( renderer&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    renderer::~renderer( )
    {
        vkDeviceWaitIdle( vk_context_.device_ );
        
        if ( vk_context_.graphics_pipeline_ != VK_NULL_HANDLE )
        {
            vkDestroyPipeline( vk_context_.device_, vk_context_.graphics_pipeline_, nullptr );
            core_info( "Vulkan -> Pipeline destroyed." );
        }
    
        if ( vk_context_.graphics_pipeline_layout_ != VK_NULL_HANDLE )
        {
            vkDestroyPipelineLayout( vk_context_.device_, vk_context_.graphics_pipeline_layout_, nullptr );
            core_info( "Vulkan -> Pipeline Layout destroyed." );
        }
    
        for ( auto &framebuffer : vk_context_.swapchain_framebuffers_ )
        {
            vkDestroyFramebuffer( vk_context_.device_, framebuffer, nullptr );
        }
        core_info( "Vulkan -> Swapchain Framebuffer destroyed." );
    
        if ( vk_context_.render_pass_ != VK_NULL_HANDLE )
        {
            vkDestroyRenderPass( vk_context_.device_, vk_context_.render_pass_, nullptr );
            core_info( "Vulkan -> Render Pass destroyed." );
        }
    
        for ( auto &image_view : vk_context_.swapchain_image_views_ )
        {
            if ( image_view != VK_NULL_HANDLE )
            {
                vkDestroyImageView( vk_context_.device_, image_view, nullptr );
            }
        }
        core_info( "Vulkan -> Swapchain Image Views destroyed" );
    
        if ( vk_context_.swapchain_ != VK_NULL_HANDLE )
        {
            vkDestroySwapchainKHR( vk_context_.device_, vk_context_.swapchain_, nullptr );
            core_info( "Vulkan -> Swapchain destroyed." );
        }
        
        if( !vk_context_.command_buffers_.empty() )
        {
            vkFreeCommandBuffers( vk_context_.device_, vk_context_.command_pool_,
                static_cast<uint32_t>( vk_context_.command_buffers_.size() ), vk_context_.command_buffers_.data() );
            core_info( "Vulkan -> Command Buffers freed." );
        }
        
        if( vk_context_.command_pool_ != VK_NULL_HANDLE )
        {
            vkDestroyCommandPool( vk_context_.device_, vk_context_.command_pool_, nullptr );
            core_info( "Vulkan -> Command Pool destroyed." );
        }
        
        for( auto& fence : vk_context_.in_flight_fences_ )
        {
            if( fence != VK_NULL_HANDLE )
            {
                vkDestroyFence( vk_context_.device_, fence, nullptr );
            }
        }
        core_info( "Vulkan -> In flight fences destroyed." );
        
        for( auto& semaphore : vk_context_.image_available_semaphores_ )
        {
            if( semaphore != VK_NULL_HANDLE )
            {
                vkDestroySemaphore( vk_context_.device_, semaphore, nullptr );
            }
        }
        core_info( "Vulkan -> Image Available Semaphores destroyed." );
        
        for( auto& semaphore : vk_context_.render_finished_semaphores_ )
        {
            if( semaphore != VK_NULL_HANDLE )
            {
                vkDestroySemaphore( vk_context_.device_, semaphore, nullptr );
            }
        }
        core_info( "Vulkan -> Render Finished Semaphores destroyed." );
    
        if ( vk_context_.device_ != VK_NULL_HANDLE )
        {
            vkDestroyDevice( vk_context_.device_, nullptr );
            core_info( "Vulkan -> Device destroyed." );
        }
    
        if constexpr ( enable_debug_layers )
        {
            if ( vk_context_.debug_report_ != VK_NULL_HANDLE )
            {
                vk_destroy_debug_callback( vk_context_.instance_, vk_context_.debug_report_, nullptr );
                core_info( "Vulkan -> Debug Report Callback destroyed." );
            }
        }
    
        if( vk_context_.instance_ != VK_NULL_HANDLE )
        {
            vkDestroyInstance( vk_context_.instance_, nullptr );
            core_info( "Vulkan -> Instance destroyed." );
        }
    }
    
    renderer& renderer::operator=( renderer&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            vk_context_.instance_ = rhs.vk_context_.instance_;
            rhs.vk_context_.instance_ = VK_NULL_HANDLE;
        
            if constexpr( enable_debug_layers )
            {
                vk_context_.debug_report_ = rhs.vk_context_.debug_report_;
                rhs.vk_context_.debug_report_ = VK_NULL_HANDLE;
            }
    
            vk_context_.surface_ = rhs.vk_context_.surface_;
            rhs.vk_context_.surface_ = VK_NULL_HANDLE;
    
            vk_context_.gpu_ = rhs.vk_context_.gpu_;
            rhs.vk_context_.gpu_ = VK_NULL_HANDLE;
    
            vk_context_.device_ = rhs.vk_context_.device_;
            rhs.vk_context_.device_ = VK_NULL_HANDLE;
    
            vk_context_.graphics_queue_ = rhs.vk_context_.graphics_queue_;
            rhs.vk_context_.graphics_queue_ = VK_NULL_HANDLE;
    
            vk_context_.present_queue_ = rhs.vk_context_.present_queue_;
            rhs.vk_context_.present_queue_ = VK_NULL_HANDLE;
            
            vk_context_.image_available_semaphores_ = std::move( rhs.vk_context_.image_available_semaphores_ );
            vk_context_.render_finished_semaphores_ = std::move( rhs.vk_context_.render_finished_semaphores_ );
            vk_context_.in_flight_fences_ = std::move( rhs.vk_context_.in_flight_fences_ );
    
            vk_context_.command_pool_ = rhs.vk_context_.command_pool_;
            rhs.vk_context_.command_pool_ = VK_NULL_HANDLE;
    
            vk_context_.command_buffers_ = std::move( rhs.vk_context_.command_buffers_ );
            
            vk_context_.surface_format_ = rhs.vk_context_.surface_format_;
            rhs.vk_context_.surface_format_ = { };
    
            vk_context_.swapchain_ = rhs.vk_context_.swapchain_;
            rhs.vk_context_.swapchain_ = VK_NULL_HANDLE;
    
            vk_context_.swapchain_image_ = std::move( rhs.vk_context_.swapchain_image_ );
            vk_context_.swapchain_image_views_ = std::move( rhs.vk_context_.swapchain_image_views_ );
    
            vk_context_.swapchain_extent_ = rhs.vk_context_.swapchain_extent_;
            rhs.vk_context_.swapchain_extent_ = { };
    
            vk_context_.render_pass_ = rhs.vk_context_.render_pass_;
            rhs.vk_context_.render_pass_ = VK_NULL_HANDLE;
    
            vk_context_.swapchain_framebuffers_ = std::move( rhs.vk_context_.swapchain_framebuffers_ );
    
            vk_context_.graphics_pipeline_layout_ = rhs.vk_context_.graphics_pipeline_layout_;
            rhs.vk_context_.graphics_pipeline_layout_ = VK_NULL_HANDLE;
    
            vk_context_.graphics_pipeline_ = rhs.vk_context_.graphics_pipeline_;
            rhs.vk_context_.graphics_pipeline_ = VK_NULL_HANDLE;
    
            vk_context_.instance_extensions_ = std::move( rhs.vk_context_.instance_extensions_ );
            vk_context_.device_extensions_ = std::move( rhs.vk_context_.device_extensions_ );
            vk_context_.validation_layers_ = std::move( rhs.vk_context_.validation_layers_ );
        }
    
        return *this;
    }
    
    
    void renderer::setup_graphics_pipeline( const TWE::renderer::graphics_pipeline_data &data )
    {
        auto vertex_shader = check_vk_return_type_result(
            create_shader_module( data.vertex_shader_filepath ),
            "create_shader_module( ) -> Vertex Shader" );
        
        core_info( "Vulkan -> Vertex Shader Module Created from: {}.", data.vertex_shader_filepath );
        
        auto fragment_shader = check_vk_return_type_result(
            create_shader_module( data.fragment_shader_filepath ),
            "create_shader_module( ) -> Fragment Shader" );
        
        core_info( "Vulkan -> Fragment Shader Module Created from: {}.", data.fragment_shader_filepath );
        
        const VkPipelineShaderStageCreateInfo shader_stages[]
            {
                {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = { },
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = vertex_shader,
                    .pName = "main",
                    .pSpecializationInfo = nullptr
                },
                {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = { },
                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = fragment_shader,
                    .pName = "main",
                    .pSpecializationInfo = nullptr
                }
            };
        
        vk_context_.graphics_pipeline_layout_ = check_vk_return_type_result(
            create_pipeline_layout( ), "create_pipeline_layout( )" );
        
        core_info( "Vulkan -> Graphics Pipeline Layout created." );
        
        vk_context_.graphics_pipeline_ = check_vk_return_type_result(
            create_graphics_pipeline( 2, shader_stages ),
            "create_graphics_pipeline( )" );
        
        core_info( "Vulkan -> Graphics Pipeline created." );
        
        vkDestroyShaderModule( vk_context_.device_, vertex_shader, nullptr );
        vkDestroyShaderModule( vk_context_.device_, fragment_shader, nullptr );
        
        core_info( "Vulkan -> Vertex Shader Module Destroyed." );
        core_info( "Vulkan -> Fragment Shader Module Destroyed." );
    }
    
    void renderer::draw_frame( )
    {
        vkWaitForFences( vk_context_.device_, 1, &vk_context_.in_flight_fences_[current_frame_],
            VK_TRUE, std::numeric_limits<uint64_t>::max() );
        vkResetFences( vk_context_.device_, 1, &vk_context_.in_flight_fences_[current_frame_] );
        
        uint32_t image_index;
        
        vkAcquireNextImageKHR( vk_context_.device_, vk_context_.swapchain_, std::numeric_limits<uint64_t>::max( ),
            vk_context_.image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &image_index );
            
        const VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        const VkSubmitInfo submit_info
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &vk_context_.image_available_semaphores_[current_frame_],
            .pWaitDstStageMask = wait_stages,
            .commandBufferCount = 1,
            .pCommandBuffers = &vk_context_.command_buffers_[image_index],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &vk_context_.render_finished_semaphores_[current_frame_]
        };
        
        try
        {
            check_vk_return_result(
                vkQueueSubmit( vk_context_.graphics_queue_, 1, &submit_info,
                    vk_context_.in_flight_fences_[current_frame_] ),
                "Failed to submit draw command buffer!" );
        }
        catch( const vk_error& e )
        {
            core_error( e.what() );
        }
        
        VkPresentInfoKHR present_info
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &vk_context_.render_finished_semaphores_[current_frame_],
            .swapchainCount = 1,
            .pSwapchains = &vk_context_.swapchain_,
            .pImageIndices = &image_index,
            .pResults = nullptr
        };
    
        try
        {
            check_vk_return_result(
                vkQueuePresentKHR( vk_context_.present_queue_, &present_info ), "Failed to present Image" );
        }
        catch( const vk_error& e )
        {
            core_error( e.what( ));
        }
    
        current_frame_ = ( ++current_frame_ ) % MAX_FRAMES_IN_FLIGHT;
    }
    
    void renderer::record_draw_calls( )
    {
        for( auto i = 0; i < vk_context_.command_buffers_.size(); ++i )
        {
            const VkCommandBufferBeginInfo begin_info
            {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
                .pInheritanceInfo = nullptr
            };
            
            check_vk_return_result(
                vkBeginCommandBuffer( vk_context_.command_buffers_[i], &begin_info ),
                "Failed to begin recording Command Buffer." );
            
            const VkClearValue clear_colour = { 0.0f, 0.0f, 0.0f, 1.0f };
            
            const VkRenderPassBeginInfo rp_begin_info
            {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext = nullptr,
                .renderPass = vk_context_.render_pass_,
                .framebuffer = vk_context_.swapchain_framebuffers_[i],
                .renderArea = { { 0, 0 }, vk_context_.swapchain_extent_ },
                .clearValueCount = 1,
                .pClearValues = &clear_colour
            };
            
            vkCmdBeginRenderPass( vk_context_.command_buffers_[i], &rp_begin_info, VK_SUBPASS_CONTENTS_INLINE );
            
            vkCmdBindPipeline( vk_context_.command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_context_.graphics_pipeline_ );
            
            vkCmdDraw( vk_context_.command_buffers_[i], 3, 1, 0, 0 );
            
            vkCmdEndRenderPass( vk_context_.command_buffers_[i] );
            
            check_vk_return_result(
                vkEndCommandBuffer( vk_context_.command_buffers_[i] ),
                "Failed to record Command Buffer" );
        }
    }
    
    void renderer::set_up( )
    {
        uint32_t supported_api_version;
        if( vkEnumerateInstanceVersion( &supported_api_version ) != VK_SUCCESS )
        {
            throw basic_error{ basic_error::flags::vk_not_supported_error, "Vulkan not Installed" };
        }
        if( supported_api_version != VK_API_VERSION_1_1 )
        {
            throw basic_error{ basic_error::flags::vk_version_error, "Vulkan 1.1 not supporetd" };
        }
        
        if constexpr( enable_debug_layers )
        {
            vk_context_.instance_extensions_.emplace_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
        }
        
        vk_context_.instance_extensions_.emplace_back( VK_KHR_SURFACE_EXTENSION_NAME );

#if defined( _WIN32 )
        vk_context_.instance_extensions_.emplace_back( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
        vk_context_.instance_extensions_.emplace_back( VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME );
#elif defined( VK_USE_PLATFORM_XCB_KHR )
        vk_context_.instance_extensions_.emplace_back( VK_KHR_XCB_SURFACE_EXTENSION_NAME );
#endif
        
        vk_context_.validation_layers_.emplace_back( "VK_LAYER_LUNARG_standard_validation" );
        
        vk_context_.device_extensions_.emplace_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );
        
        if( !check_instance_extension_support( vk_context_.instance_extensions_ ) )
        {
            throw basic_error{ basic_error::flags::vk_instance_ext_support_error, "Instance extensions requested, but not supporetd" };
        }
        
        if constexpr ( !enable_debug_layers )
        {
            if( !check_debug_layer_support( vk_context_.validation_layers_ ) )
            {
                throw basic_error{ basic_error::flags::vk_validation_layer_support_error, "VK_LAYER_LUNARG_standard_validation not supported" };
            }
        }
    }
    
    const vk_return_type<VkInstance> renderer::create_instance( const std::string& app_name,
        uint32_t app_version ) const noexcept
    {
        VkInstance instance = VK_NULL_HANDLE;
        
        const VkApplicationInfo app_info =
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = app_name.c_str( ),
            .applicationVersion = app_version,
            .pEngineName = "The Wombat Engine",
            .engineVersion = VK_MAKE_VERSION( 0, 0, 0 ),
            .apiVersion = VK_API_VERSION_1_1,
        };
        
        if constexpr( enable_debug_layers )
        {
            const VkInstanceCreateInfo create_info =
            {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = { },
                .pApplicationInfo = &app_info,
                .enabledLayerCount = static_cast<uint32_t>( vk_context_.validation_layers_.size() ),
                .ppEnabledLayerNames = vk_context_.validation_layers_.data(),
                .enabledExtensionCount = static_cast<uint32_t>( vk_context_.instance_extensions_.size() ),
                .ppEnabledExtensionNames = vk_context_.instance_extensions_.data(),
            };
            
            return { vkCreateInstance( &create_info, nullptr, &instance ), instance };
        }
        else
        {
            const VkInstanceCreateInfo create_info =
            {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = { },
                .pApplicationInfo = &app_info,
                .enabledLayerCount = 0,
                .ppEnabledLayerNames = nullptr,
                .enabledExtensionCount = static_cast<uint32_t>( vk_context_.instance_extensions_.size() ),
                .ppEnabledExtensionNames = vk_context_.instance_extensions_.data(),
            };
            
            return { vkCreateInstance( &create_info, nullptr, &instance ), instance };
        }
    }
    
    const vk_return_type<VkDebugReportCallbackEXT> renderer::create_debug_report( ) const noexcept
    {
        VkDebugReportCallbackEXT debug_report;
        
        const VkDebugReportCallbackCreateInfoEXT create_info
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                VK_DEBUG_REPORT_ERROR_BIT_EXT,
            .pfnCallback = debug_callback_function,
            .pUserData = nullptr
        };
        
        return { vk_create_debug_callback( vk_context_.instance_, &create_info, nullptr, &debug_report ), debug_report };
    }
    
    const vk_return_type<VkSurfaceKHR> renderer::create_surface( const window& wnd ) const noexcept
    {
        return wnd.create_surface( vk_context_.instance_ );
    }
    
    const VkPhysicalDevice renderer::pick_physical_device( ) const noexcept
    {
        VkPhysicalDevice device = VK_NULL_HANDLE;
        
        std::multimap<uint32_t, VkPhysicalDevice> candidates;
        
        uint32_t num_devices = 0;
        vkEnumeratePhysicalDevices( vk_context_.instance_, &num_devices, nullptr );
        
        std::vector<VkPhysicalDevice> available_devices( num_devices );
        vkEnumeratePhysicalDevices( vk_context_.instance_, &num_devices, available_devices.data( ));
        
        for ( auto &physical_device : available_devices )
        {
            if ( is_physical_device_suitable( vk_context_.surface_, physical_device, vk_context_.device_extensions_ ) )
            {
                uint32_t score = 0;
                
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties( physical_device, &properties );
                
                if ( properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
                {
                    score += 2;
                }
                else if ( properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU )
                {
                    score += 1;
                }
                else
                {
                    score += 0;
                }
                
                candidates.insert( { score, physical_device } );
            }
        }
        
        if ( candidates.rbegin( )->first > 0 )
        {
            return candidates.rbegin()->second;
        }
        else
        {
            return VK_NULL_HANDLE;
        }
    }
    
    const vk_return_type<VkDevice> renderer::create_device( ) const noexcept
    {
        VkDevice device;
        
        auto queue_families = find_queue_family_indices( vk_context_.surface_, vk_context_.gpu_ );
        std::set<uint32_t> unique_queue_family;
        
        if ( queue_families.graphic_family_.has_value() )
        {
            unique_queue_family.insert( queue_families.graphic_family_.value() );
        }
        
        if( queue_families.compute_family_.has_value() )
        {
            unique_queue_family.insert( queue_families.compute_family_.value() );
        }
        
        if( queue_families.present_family_.has_value() )
        {
            unique_queue_family.insert( queue_families.present_family_.value() );
        }
        
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        queue_create_infos.reserve( unique_queue_family.size() );
        
        float priority = 1.0f;
        for( const auto& queue_family : unique_queue_family )
        {
            const VkDeviceQueueCreateInfo create_info
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = { },
                .queueFamilyIndex = queue_family,
                .queueCount = 1,
                .pQueuePriorities = &priority
            };
            
            queue_create_infos.emplace_back( create_info );
        }
        
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures( vk_context_.gpu_, &features );

        if constexpr( enable_debug_layers )
        {
            const VkDeviceCreateInfo create_info
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .pNext = nullptr,
                .flags = { },
                .queueCreateInfoCount = static_cast<uint32_t>( queue_create_infos.size() ),
                .pQueueCreateInfos = queue_create_infos.data(),
                .enabledLayerCount = static_cast<uint32_t>( vk_context_.validation_layers_.size( ) ),
                .ppEnabledLayerNames = vk_context_.validation_layers_.data(),
                .enabledExtensionCount = static_cast<uint32_t>( vk_context_.device_extensions_.size() ),
                .ppEnabledExtensionNames = vk_context_.device_extensions_.data(),
                .pEnabledFeatures = &features
            };
    
            return { vkCreateDevice( vk_context_.gpu_, &create_info, nullptr, &device), device };
        }
        else
        {
            const VkDeviceCreateInfo create_info
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .pNext = nullptr,
                .flags = { },
                .queueCreateInfoCount = static_cast<uint32_t>( queue_create_infos.size() ),
                .pQueueCreateInfos = queue_create_infos.data(),
                .enabledLayerCount = 0,
                .ppEnabledLayerNames = nullptr,
                .enabledExtensionCount = static_cast<uint32_t>( vk_context_.device_extensions_.size() ),
                .ppEnabledExtensionNames = vk_context_.device_extensions_.data(),
                .pEnabledFeatures = &features
            };
    
            return { vkCreateDevice( vk_context_.gpu_, &create_info, nullptr, &device), device };
        }
    }
    
    const vk_return_type<VkSemaphore> renderer::create_semaphore( ) const noexcept
    {
        VkSemaphore semaphore_ = VK_NULL_HANDLE;
        
        const VkSemaphoreCreateInfo create_info
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = { }
        };
        
        return { vkCreateSemaphore( vk_context_.device_, &create_info, nullptr, &semaphore_ ), semaphore_ };
    }
    
    const vk_return_type<VkFence> renderer::create_fence( ) const noexcept
    {
        VkFence fence = VK_NULL_HANDLE;
        
        const VkFenceCreateInfo create_info
        {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };
        
        return { vkCreateFence( vk_context_.device_, &create_info, nullptr, &fence ), fence };
    }
    
    const vk_return_type<VkCommandPool> renderer::create_command_pool( uint32_t queue_family ) const noexcept
    {
        VkCommandPool command_pool = VK_NULL_HANDLE;
        
        const VkCommandPoolCreateInfo create_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
            .queueFamilyIndex = queue_family
        };
        
        return { vkCreateCommandPool( vk_context_.device_, &create_info, nullptr, &command_pool ), command_pool };
    }
    
    const vk_return_type<std::vector<VkCommandBuffer>> renderer::create_command_buffers( uint32_t count ) const noexcept
    {
        std::vector<VkCommandBuffer> command_buffers( count );
        
        const VkCommandBufferAllocateInfo allocate_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = vk_context_.command_pool_,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = count
        };
        
        return { vkAllocateCommandBuffers( vk_context_.device_, &allocate_info, command_buffers.data() ), command_buffers };
    }

    const vk_return_type<VkSwapchainKHR> renderer::create_swapchain( const queue_family_indices_type& queue_family_indices_,
        const VkPresentModeKHR& present_mode_, const VkSurfaceCapabilitiesKHR& capabilities_,
        uint32_t image_count_ ) const noexcept
    {
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    
        const uint32_t queue_family_indices[] = {
            queue_family_indices_.graphic_family_.value( ),
            queue_family_indices_.present_family_.value( )
        };
    
        if ( queue_family_indices_.graphic_family_ !=
             queue_family_indices_.present_family_ )
        {
            const VkSwapchainCreateInfoKHR create_info
            {
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .pNext = nullptr,
                .flags = { },
                .surface = vk_context_.surface_,
                .minImageCount = image_count_,
                .imageFormat = vk_context_.surface_format_.format,
                .imageColorSpace = vk_context_.surface_format_.colorSpace,
                .imageExtent = vk_context_.swapchain_extent_,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .imageSharingMode = VK_SHARING_MODE_CONCURRENT,
                .queueFamilyIndexCount = 2,
                .pQueueFamilyIndices = queue_family_indices,
                .preTransform = capabilities_.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = present_mode_,
                .clipped = VK_TRUE,
                .oldSwapchain = VK_NULL_HANDLE
            };
        
            return { vkCreateSwapchainKHR( vk_context_.device_, &create_info, nullptr, &swapchain ), swapchain };
        }
        else
        {
            const VkSwapchainCreateInfoKHR create_info
            {
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .pNext = nullptr,
                .flags = { },
                .surface = vk_context_.surface_,
                .minImageCount = image_count_,
                .imageFormat = vk_context_.surface_format_.format,
                .imageColorSpace = vk_context_.surface_format_.colorSpace,
                .imageExtent = vk_context_.swapchain_extent_,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .queueFamilyIndexCount = 0,
                .pQueueFamilyIndices = nullptr,
                .preTransform = capabilities_.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = present_mode_,
                .clipped = VK_TRUE,
                .oldSwapchain = VK_NULL_HANDLE
            };
        
            return { vkCreateSwapchainKHR( vk_context_.device_, &create_info, nullptr, &swapchain ), swapchain };
        }
    }

    const vk_return_type<VkImageView> renderer::create_image_view( const VkImage& image ) const noexcept
    {
        VkImageView image_view = VK_NULL_HANDLE;
    
        const VkComponentMapping mapping
        {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY
        };
        const VkImageSubresourceRange subresource_range
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        };
        const VkImageViewCreateInfo create_info
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = vk_context_.surface_format_.format,
            .components = mapping,
            .subresourceRange = subresource_range
        };
    
        return { vkCreateImageView( vk_context_.device_, &create_info, nullptr, &image_view ), image_view };
    }
    
    const vk_return_type<VkFramebuffer> renderer::create_framebuffer( const VkImageView& image_view ) const noexcept
    {
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        
        const VkFramebufferCreateInfo create_info
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
            .renderPass = vk_context_.render_pass_,
            .attachmentCount = 1,
            .pAttachments = &image_view,
            .width = vk_context_.swapchain_extent_.width,
            .height = vk_context_.swapchain_extent_.height,
            .layers = 1
        };
        
        return { vkCreateFramebuffer( vk_context_.device_, &create_info, nullptr, &framebuffer ), framebuffer };
    }
    
    const vk_return_type<VkRenderPass> renderer::create_render_pass( ) const noexcept
    {
        VkRenderPass render_pass = VK_NULL_HANDLE;
        
        const VkAttachmentDescription colour_attachment
        {
            .flags = { },
            .format = vk_context_.surface_format_.format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };
        
        const VkAttachmentReference colour_attachment_ref
        {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };
        
        const VkSubpassDescription subpass_description
        {
            .flags = { },
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount = 0,
            .pInputAttachments = nullptr,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colour_attachment_ref,
            .pResolveAttachments = nullptr,
            .pDepthStencilAttachment = nullptr,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments = nullptr
        };
    
        const VkSubpassDependency dependency
        {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = { }
        };
        
        const VkRenderPassCreateInfo create_info
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
            .attachmentCount = 1,
            .pAttachments = &colour_attachment,
            .subpassCount = 1,
            .pSubpasses = &subpass_description,
            .dependencyCount = 1,
            .pDependencies = &dependency,
        };
        
        return { vkCreateRenderPass( vk_context_.device_, &create_info, nullptr, &render_pass ), render_pass };
    }
    
    const vk_return_type<VkShaderModule> renderer::create_shader_module( const std::string& filepath ) const noexcept
    {
        VkShaderModule shader_module = VK_NULL_HANDLE;
        
        const std::string shader_code = read_from_binary_file( filepath );
        
        const VkShaderModuleCreateInfo create_info
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
            .codeSize = shader_code.size(),
            .pCode = reinterpret_cast<const uint32_t*>( shader_code.data() )
        };
        
        return { vkCreateShaderModule( vk_context_.device_, &create_info, nullptr, &shader_module ), shader_module };
    }
    
    const vk_return_type<VkPipelineLayout> renderer::create_pipeline_layout( ) const noexcept
    {
        VkPipelineLayout layout = VK_NULL_HANDLE;
        
        const VkPipelineLayoutCreateInfo create_info
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .pNext = nullptr,
                .flags = { },
                .setLayoutCount = 0,
                .pSetLayouts = nullptr,
                .pushConstantRangeCount = 0,
                .pPushConstantRanges = nullptr
            };
        
        return { vkCreatePipelineLayout( vk_context_.device_, &create_info, nullptr, &layout ), layout };
    }
    
    const vk_return_type<VkPipeline> renderer::create_graphics_pipeline( std::uint32_t stage_count,
        const VkPipelineShaderStageCreateInfo* p_stages ) const noexcept
    {
        VkPipeline pipeline = VK_NULL_HANDLE;
        
        const VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = nullptr
        };
        const VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE          // Test with VK_TRUE and VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
        };
        const VkViewport viewport
        {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>( vk_context_.swapchain_extent_.width ),
            .height = static_cast<float>( vk_context_.swapchain_extent_.height ),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };
        const VkRect2D scissors
        {
            .offset = { 0, 0 },
            .extent = vk_context_.swapchain_extent_
        };
        const VkPipelineViewportStateCreateInfo viewport_state_create_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissors
        };
        const VkPipelineRasterizationStateCreateInfo rasterization_state_create_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
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
        const VkPipelineMultisampleStateCreateInfo multisample_state_create_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE
        };
        const VkPipelineColorBlendAttachmentState colour_blend_attachment_state
        {
            .blendEnable = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
        };
        const VkPipelineColorBlendStateCreateInfo colour_blend_state_create_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &colour_blend_attachment_state,
            .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
        };
        const VkDynamicState dynamic_states[]
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };
        const VkPipelineDynamicStateCreateInfo dynamic_state_create_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
            .dynamicStateCount = 0,
            .pDynamicStates = nullptr
        };
        
        const VkGraphicsPipelineCreateInfo create_info
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = { },
            .stageCount = stage_count,
            .pStages = p_stages,
            .pVertexInputState = &vertex_input_state_create_info,
            .pInputAssemblyState = &input_assembly_state_create_info,
            .pTessellationState = nullptr,
            .pViewportState = &viewport_state_create_info,
            .pRasterizationState = &rasterization_state_create_info,
            .pMultisampleState = &multisample_state_create_info,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colour_blend_state_create_info,
            .pDynamicState = &dynamic_state_create_info,
            .layout = vk_context_.graphics_pipeline_layout_,
            .renderPass = vk_context_.render_pass_,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
        };
        
        return { vkCreateGraphicsPipelines( vk_context_.device_, nullptr, 1, &create_info, nullptr, &pipeline ), pipeline };
    }
    
    bool renderer::check_instance_extension_support( const std::vector<const char*>& instance_extensions ) const noexcept
    {
        uint32_t count;
        vkEnumerateInstanceExtensionProperties( nullptr, &count, nullptr );
        
        std::vector<VkExtensionProperties> available_extensions( count );
        vkEnumerateInstanceExtensionProperties( nullptr, &count, available_extensions.data() );
        
        for( const auto& extension : instance_extensions )
        {
            bool is_supported = false;
            
            for( const auto& extension_property : available_extensions )
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
    
    bool renderer::check_debug_layer_support( const std::vector<const char*>& debug_layers ) const noexcept
    {
        uint32_t count;
        vkEnumerateInstanceLayerProperties( &count, nullptr );
        
        std::vector<VkLayerProperties> available_layers( count );
        vkEnumerateInstanceLayerProperties( &count, available_layers.data() );
        
        for( const auto& layer : debug_layers )
        {
            bool is_supported = false;
            
            for( const auto& layer_properties : available_layers )
            {
                if( strcmp( layer, layer_properties.layerName ) == 0 )
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
    
    bool renderer::is_physical_device_suitable( const VkSurfaceKHR& surface, const VkPhysicalDevice& physical_device,
        const std::vector<const char*>& device_extensions ) const noexcept
    {
        auto indices = find_queue_family_indices( surface, physical_device );
        
        return indices.is_complete() &&
               check_physical_device_extension_support( physical_device, device_extensions ) &&
               is_swapchain_adequate( surface, physical_device );
    }
    
    bool renderer::check_physical_device_extension_support( const VkPhysicalDevice &physical_device,
        const std::vector<const char*>& device_extensions ) const noexcept
    {
        std::set<std::string> required_extensions( device_extensions.cbegin(), device_extensions.cend() );
        
        uint32_t count;
        vkEnumerateDeviceExtensionProperties( physical_device, nullptr, &count, nullptr );
        
        std::vector<VkExtensionProperties> properties( count );
        vkEnumerateDeviceExtensionProperties( physical_device, nullptr, &count, properties.data() );
        
        for( const auto& property : properties )
        {
            required_extensions.erase( property.extensionName );
        }
        
        return required_extensions.empty();
    }
    
    const renderer::queue_family_indices_type renderer::find_queue_family_indices(
        const VkSurfaceKHR &surface,
        const VkPhysicalDevice &physical_device ) const noexcept
    {
        queue_family_indices_type indices;
        
        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &count, nullptr );
        
        std::vector<VkQueueFamilyProperties> queue_properties( count );
        vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &count, queue_properties.data() );
        
        int i = 0;
        for( const auto& queue_property : queue_properties )
        {
            if( queue_property.queueCount > 0 )
            {
                if ( queue_property.queueFlags & VK_QUEUE_GRAPHICS_BIT )
                {
                    indices.graphic_family_ = i;
                }
                if ( queue_property.queueFlags & VK_QUEUE_COMPUTE_BIT )
                {
                    indices.compute_family_ = i;
                }
                
                VkBool32 present_support;
                vkGetPhysicalDeviceSurfaceSupportKHR( physical_device, i, surface, &present_support );
                
                if ( present_support )
                {
                    indices.present_family_ = i;
                }
            }
            
            ++i;
        }
        
        return indices;
    }
    
    bool renderer::is_swapchain_adequate( const VkSurfaceKHR& surface, const VkPhysicalDevice& physical_device ) const noexcept
    {
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR( physical_device, surface, &format_count, nullptr );
        
        std::vector<VkSurfaceFormatKHR> surface_formats( format_count );
        vkGetPhysicalDeviceSurfaceFormatsKHR( physical_device, surface, &format_count, surface_formats.data() );
        
        
        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR( physical_device, surface, &present_mode_count, nullptr );
        
        std::vector<VkPresentModeKHR> present_modes( present_mode_count );
        vkGetPhysicalDeviceSurfacePresentModesKHR( physical_device, surface, &present_mode_count, present_modes.data() );
        
        return !surface_formats.empty() && !present_modes.empty();
    }
    
    const renderer::swapchain_support_details_type renderer::query_swapchain_support( const VkSurfaceKHR& surface,
        const VkPhysicalDevice& physical_device ) const noexcept
    {
        swapchain_support_details_type details;

        /* Get the capabilities. */
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physical_device, surface, &details.capabilities_ );

        /* Get the number of formats. */
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR( physical_device, surface, &format_count, nullptr );

        /* if more than 0 formats, resize and save them in details. */
        if( format_count != 0 )
        {
            details.formats_.resize( format_count );
            vkGetPhysicalDeviceSurfaceFormatsKHR( physical_device, surface, &format_count, details.formats_.data() );
        }

        /* Get the number of present modes. */
        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR( physical_device, surface, &present_mode_count, nullptr );

        /* If not 0, resize and save them in details. */
        if( present_mode_count != 0 )
        {
            details.present_modes_.resize( present_mode_count );
            vkGetPhysicalDeviceSurfacePresentModesKHR( physical_device, surface, &present_mode_count, details.present_modes_.data() );
        }

        return details;
    }

    const VkSurfaceFormatKHR renderer::choose_swapchain_surface_format(
            const std::vector<VkSurfaceFormatKHR> &available_formats ) const noexcept
    {
        if ( available_formats.size( ) == 1 && available_formats[0].format == VK_FORMAT_UNDEFINED )
        {
            return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }
    
        const auto iter = std::find_if( available_formats.cbegin( ), available_formats.cend( ),
                                        []( const VkSurfaceFormatKHR &format )
                                        {
                                            return format.format == VK_FORMAT_B8G8R8A8_UNORM &&
                                                   format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
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

    const VkPresentModeKHR renderer::choose_swapchain_present_mode(
            const std::vector<VkPresentModeKHR> &available_present_modes ) const noexcept
    {
        const auto iter = std::find_if( available_present_modes.cbegin( ), available_present_modes.cend( ),
                                        []( const VkPresentModeKHR &present_mode )
                                        {
                                            return present_mode == VK_PRESENT_MODE_MAILBOX_KHR;
                                        } );
    
        if ( iter != available_present_modes.cend( ) )
        {
            return *iter;
        }
        else
        {
            return VK_PRESENT_MODE_FIFO_KHR;
        }
    }

    const VkExtent2D renderer::choose_swapchain_extent( const VkSurfaceCapabilitiesKHR &capabilities ) const noexcept
    {
        if ( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max( ) )
        {
            return capabilities.currentExtent;
        }
        else
        {
            const auto width = std::max( capabilities.minImageExtent.width,
                                         std::min( capabilities.maxImageExtent.width, window_width_ ) );
        
            const auto height = std::max( capabilities.minImageExtent.height,
                                          std::min( capabilities.maxImageExtent.height, window_height_ ) );
        
            return { width, height };
        }
    }
}