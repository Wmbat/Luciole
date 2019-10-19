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

/* HEADER GUARDS */
#ifndef LUCIOLE_CONTEXT_HPP
#define LUCIOLE_CONTEXT_HPP

/* INCLUDES */
#include <luciole/luciole_core.hpp>
#include <luciole/ui/window.hpp>
#include <luciole/vk/core.hpp>
#include <luciole/vk/errors.hpp>
#include <luciole/vk/extension.hpp>
#include <luciole/vk/layer.hpp>
#include <luciole/vk/queue.hpp>

#include <spdlog/logger.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <cstdint>
#include <vector>
#include <optional>
#include <unordered_map>
#include <variant>

/**
 * @brief A Vulkan context to handle all
 * base vulkan object.
 */
class context
{
private:
   struct command_pool
   {
       VkCommandPool handle_ = VK_NULL_HANDLE;
       queue::flag flags_ = queue::flag::e_none;
   };

   using command_pools_container_t = std::unordered_map<std::uint32_t, context::command_pool>;

   /* STRONG TYPES */
   struct extensions_parameter{ };
   using extensions_t = strong_type<std::vector<vk::extension>, extensions_parameter>;

   struct extension_names_parameter{ };
   using extension_names_t = strong_type<std::vector<std::string>, extension_names_parameter>;

   struct layers_parameter{ };
   using layers_t = strong_type<std::vector<vk::layer>, layers_parameter>;

   struct layer_names_parameter{ };
   using layer_names_t = strong_type<std::vector<std::string>, layer_names_parameter>;

   struct queue_properties_parameter{ };
   using queue_properties_t = strong_type<std::vector<VkQueueFamilyProperties>, queue_properties_parameter>;

public:
   /**
    * @brief Default Constructor.
    */
   context( ) = default;
   /**
    * @brief Constructor.
    */
   explicit context( const ui::window& wnd );
   /**
    * @brief Delete Copy Constructor.
    */
   context( const context& other ) = delete;
   /**
    * @brief Move Constructor.
    */
   context( context&& other );
   /**
    * @brief Destructor.
    */
   ~context( );
   
   /**
    * @brief Deleted Copy Assigment Operator.
    */
   context& operator=( context const& rhs ) = delete;
   
   /**
    * @brief Move Assigment Operator.
    */
   context& operator=( context&& rhs );
  
   /**
    * @brief Create and fill all info that the context
    * can fill in a SwapchainCreateInfo struct.
    *
    * @return The partially filled struct
    */
   [[nodiscard]]
   VkSwapchainCreateInfoKHR swapchain_create_info( 
   ) const noexcept PURE;

   /**
    * @brief Create a swapchain.
    *
    * @param [in] create_info The information required to create
    * the swapchain.
    *
    * @return Either a handle to the newly created 
    * swapchain or an error code.
    */
   [[nodiscard]] 
   std::variant<VkSwapchainKHR, vk::error> create_swapchain( 
      vk::swapchain_create_info_t const& create_info 
   ) const PURE;
   
   /**
    * @brief Destroy a swapchain.
    *
    * @param [in] swapchain The handle to the swapchain.
    */
   void destroy_swapchain( 
      vk::swapchain_t swapchain 
   ) const noexcept PURE;
  
   /**
    * @brief Create an image view.
    *
    * @param [in] create_info The information required to create the
    * image view.
    *
    * @return Either a handle to the newly created image view or
    * an error code explaining why the creation failed.
    */
   [[nodiscard]]
   std::variant<VkImageView, vk::error> create_image_view( 
      vk::image_view_create_info_t const& create_info
   ) const noexcept PURE;
   
   /**
    * @brief Destroy an image view.
    *
    * @param [in] image_view The handle to the image view. 
    */
   void destroy_image_view( 
      vk::image_view_t image_view 
   ) const noexcept PURE;
   
   /**
    * @brief Create a render pass.
    *
    * @param [in] create_info The information required for the creation
    * of the render pass.
    *
    * @return Either a handle to the newly created render pass or
    * an error code explaining why the creation failed.
    */
   [[nodiscard]] 
   std::variant<VkRenderPass, vk::error> create_render_pass( 
      vk::render_pass_create_info_t const& create_info 
   ) const noexcept PURE;
    
   /**
    * @brief Destroy a render pass.
    *
    * @param render_pass The handle to the render pass.
    */
   void destroy_render_pass( 
      vk::render_pass_t render_pass 
   ) const noexcept PURE;
   
   /**
    * @brief Create a pipeline layout.
    *
    * @param create_info The information required to create the
    * pipeline layout.
    * @return Either a handle to the newly created pipeline layout
    * or an error code.
    */
   [[nodiscard]] 
   std::variant<VkPipelineLayout, vk::error> create_pipeline_layout( 
      vk::pipeline_layout_create_info_t const& create_info 
   ) const noexcept PURE;

   /**
    * @brief Destroy a pipeline layout.
    *
    * @param pipeline_layout The handle to the pipeline
    * layout.
    */
   void destroy_pipeline_layout( 
      vk::pipeline_layout_t pipeline_layout 
   ) const noexcept PURE;

   /**
    * @brief Create a graphics pipeline.
    *
    * @param create_info The information required to create the
    * graphics pipeline.
    * @return Either a handle to the newly created graphics
    * pipeline or an error code.
    */
   [[nodiscard]]
   std::variant<VkPipeline, vk::error> create_pipeline(
      vk::graphics_pipeline_create_info_t const& create_info 
   ) const noexcept PURE;

   /**
    * @brief Create a compute pipeline.
    *
    * @param create_info The information required to create the
    * compute pipeline.
    * @return Either a handle to the newly created compute
    * pipeline or an error code.
    */
   [[nodiscard]] 
   std::variant<VkPipeline, vk::error> create_pipeline(
      vk::compute_pipeline_create_info_t const& create_info 
   ) const noexcept PURE;

   /**
    * @brief Destroy a pipeline.
    *
    * @param pipeline The handle to the pipeline.
    */
   void destroy_pipeline(
      vk::pipeline_t pipeline 
   ) const noexcept PURE;

   /**
    * @brief Create a shader module.
    *
    * @param create_info The information required to create
    * a shader module.
    * @return A handle to the newly created shader module.
    */
   [[nodiscard]]
   VkShaderModule create_shader_module( 
      vk::shader_module_create_info_t const& create_info 
   ) const noexcept PURE;

   /**
    * @brief Destroy a shader module.
    *
    * @param shader_module The handle to the shader module.
    */
   void destroy_shader_module( 
      vk::shader_module_t shader_module 
   ) const noexcept PURE;

   /**
    * @brief Create a framebuffer.
    *
    * @param create_info The information required to create the
    * framebuffer.
    * @return Either a handle to the newly create framebuffer or
    * an error code.
    */
   [[nodiscard]] 
   std::variant<VkFramebuffer, vk::error> create_framebuffer( 
      vk::framebuffer_create_info_t const& create_info
   ) const noexcept PURE;

   /**
    * @brief Destroy a framebuffer.
    *
    * @param framebuffer The handle to the framebuffer.
    */
   void destroy_framebuffer( 
      vk::framebuffer_t framebuffer 
   ) const noexcept PURE;

   /**
    * @brief Create a semaphore.
    *
    * @param create_info The information required to create
    * the semphore.
    * @return Either a handle to the newly created semaphore
    * or an error code.
    */
   [[nodiscard]] 
   std::variant<VkSemaphore, vk::error> create_semaphore( 
      vk::semaphore_create_info_t const& create_info 
   ) const noexcept PURE;

   /**
    * @brief Destroy a semaphore.
    *
    * @param semaphore The handle to the semaphore.
    */
   void destroy_semaphore( 
      vk::semaphore_t semaphore 
   ) const noexcept PURE;

   /**
    * @brief Create a fence.
    *
    * @param create_info The information required to
    * create a fence.
    * @return Either a handle to the newly create fence
    * or an error code.
    */
   [[nodiscard]]
   std::variant<VkFence, vk::error> create_fence( 
      vk::fence_create_info_t const& create_info 
   ) const noexcept PURE;

   /**
    * @brief Destroy a fence.
    *
    * @param The handle to the fence.
    */
   void destroy_fence( 
      vk::fence_t fence 
   ) const noexcept PURE;

   /**
    * @brief Create an array of command buffers.
    *
    * @param flag The queue the command buffer should
    * be associated with.
    * @param buffer_count The number of command buffers
    * to create.
    * @return Either a vector of handle to the newly created 
    * command buffers or an error code.
    */
   [[nodiscard]] 
   std::variant<std::vector<VkCommandBuffer>, vk::error> create_command_buffers( 
      queue::flag_t flag, 
      count32_t buffer_count 
   ) const PURE;

   /**
    * @brief Get the swapchain images from the swapchain.
    *
    * @param swapchain The swapchain to get the images from.
    * @param image_count The number of images we want.
    * @return Either a vector of image handles to the images
    * or an error code.
    */
   [[nodiscard]] 
   std::variant<std::vector<VkImage>, vk::error> get_swapchain_images( 
      vk::swapchain_t swapchain, 
      count32_t image_count 
   ) const PURE;
   
   /**
    * @brief Get the surface capabilities.
    */
   [[nodiscard]]
   VkSurfaceCapabilitiesKHR get_surface_capabilities( 
   ) const noexcept PURE;
    
   /**
    * @brief Find all the queue family indices used in the context.
    * 
    * @return std::vector<std::uint32_t> A vector containing all the unique family indices.
    */
   [[nodiscard]] 
   std::vector<std::uint32_t> get_unique_family_indices( 
   ) const PURE;

   /**
    * @brief Get all the formats supported by the surface.
    *
    * @return The supported formats.
    */
   [[nodiscard]] 
   std::vector<VkSurfaceFormatKHR> get_surface_format( 
   ) const PURE;
    
   /**
    * @brief Get all the present modes supported by the
    *
    * @return the available present modes.
    */
   [[nodiscard]] 
   std::vector<VkPresentModeKHR> get_present_modes( 
   ) const PURE;

   /**
    * @brief Get the window extent.
    */
   [[nodiscard]] 
   VkExtent2D get_window_extent( ) const PURE;
   
   /**
    * @brief Wait on a queue do finish it's task.
    *
    * @param flag The type of queue.
    * @return The status of the operation.
    */
   vk::error queue_wait_idle(
      queue::flag_t flag
   ) const PURE;

   /**
    * @brief Submit a queue.
    *
    * @param flag The flags to submit with the queue.
    * @param submit_info The information needed to submit
    * the queue
    * @param fence A fence for synchronization.
    * @return Returns the status of the operation.
    */
   [[nodiscard]] 
   vk::error submit_queue( 
      queue::flag_t flag, 
      vk::submit_info_t const& submit_info, 
      vk::fence_t fence 
   ) const noexcept PURE;

   /**
    * @brief Present a queue.
    *
    * @param flag The flags to submit the queue with.
    * @param present_info The information required to 
    * present the queue.
    * @return The status of the operation.
    */
   [[nodiscard]] 
   vk::error present_queue( 
      queue::flag_t flag, 
      vk::present_info_t const& present_info 
   ) const noexcept PURE;

   /**
    * @brief Wait for a fence.
    *
    * @param fence The handle to the fence.
    */
   void wait_for_fence( 
      vk::fence_t fence 
   ) const noexcept;

   /**
    * @brief Reset a fence.
    *
    * @param fence The handle to the fence.
    */
   void reset_fence( 
      vk::fence_t fence 
   ) const noexcept;

   vk::error device_wait_idle(
   ) const noexcept PURE;

   //TODO: Fix this
   VkDevice get( ) const
   {
      return device_;
   }

   VmaAllocator get_memory_allocator( 
   ) const PURE;

private:
   /**
    * @brief Load all the validation layers.
    */
   [[nodiscard]] 
   std::vector<vk::layer> load_validation_layers( 
   ) const PURE;
    
   /**
    * @brief Load all the instance extensions
    */
   [[nodiscard]] 
   std::vector<vk::extension> load_instance_extensions(
   ) const PURE;

   /**
    * @brief Load all the device extensions.
    */
   [[nodiscard]] 
   std::vector<vk::extension> load_device_extensions(
   ) const PURE;

   /**
    * @brief Find all the validation layers supported by
    * the instance.
    */
   [[nodiscard]] 
   std::vector<std::string> check_layer_support( 
      const layers_t& layers
   ) const PURE;

   /**
    * @brief Find all the extension that are supported.
    */
   [[nodiscard]] 
   std::vector<std::string> check_ext_support( 
      extensions_t const& extensions 
   ) const PURE;

   /**
    * @brief Create an instance.
    *
    * @param app_info Informatio about the application.
    * @param enabled_ext_name The names of the extensions
    * we want to enable.
    * @param enabled_layer_names The names of the validation
    * layers we want to enable.
    * @return A handle to the newly created instance or an
    * error code.
    */
   [[nodiscard]] 
   std::variant<VkInstance, vk::error> create_instance( 
      VkApplicationInfo const& app_info, 
      extension_names_t const& enabled_ext_name, 
      layer_names_t const& enabled_layer_names 
   ) const PURE;
    
   /**
    * @brief Create a debug messenger.
    *
    * @return The handle to the newly created debug messenger
    * or an erro code.
    */
   [[nodiscard]] 
   std::variant<VkDebugUtilsMessengerEXT, vk::error> create_debug_messenger( 
   ) const PURE;

   /**
    * @brief Create a surface.
    *
    * @param wnd The window we create the surface
    * from.
    * @return The handle to the newly created surface handle 
    * or an error code
    */
   [[nodiscard]] 
   std::variant<VkSurfaceKHR, vk::error> create_surface( 
      ui::window const& wnd 
   ) const PURE;
    
   /**
    * @brief Pick which gpu to use.
    *
    * @return The handle to the selected gpu or
    * an error code.
    */
   [[nodiscard]] 
   std::variant<VkPhysicalDevice, vk::error> pick_gpu(
   ) const PURE;
   
   /**
    * @brief Create a logical device.
    * 
    * @param enabled_ext_names The names of the enabled extensions
    * we want to use.
    * @param queue_properties The properties of the queue families
    * supported by the gpu.
    * @return Either a handle to the newly cerated logical device
    * or an error code.
    */
   [[nodiscard]] 
   std::variant<VkDevice, vk::error> create_device( 
      extension_names_t const& enabled_ext_name, 
      queue_properties_t const& queue_properties 
   ) const PURE; 

   /**
    * @brief Create a VmaAllocator.
    *
    * @return The newly created allocator
    */
   [[nodiscard]]
   std::variant<VmaAllocator, vk::error> create_memory_allocator(
   ) const PURE;

   /**
    * @brief Get an unordered_map of queues.
    *
    * @return The unordered_map of queues.
    */
   [[nodiscard]] 
   std::unordered_map<queue::flag, queue> get_queues( 
      queue_properties_t const& queue_properties 
   ) const PURE;
   
   /**
    * @brief Create a unordered_map of command pools.
    *
    * @return Either the unordered_map of command pools or 
    * an error code.
    */
   [[nodiscard]] 
   std::variant<command_pools_container_t, vk::error> create_command_pools( 
   ) const PURE;

   /**
    * @brief Rate a gpu based on what it is capable
    * of doing and what it supports.
    *
    * @param gpu The handle to the gpu.
    * @return The rating given to the gpu.
    */
   int rate_gpu( 
      vk::physical_device_t const gpu 
   ) const PURE;

   /**
    * @brief Retrieve the properties of the queue families.
    *
    * @return An array of VkQueueFamilyProperties
    */
   std::vector<VkQueueFamilyProperties> query_queue_family_properties( 
   ) const PURE;

private:
   glm::u32vec2 wnd_size_;
    
   VkInstance instance_ = VK_NULL_HANDLE;
   VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
   VkSurfaceKHR surface_ = VK_NULL_HANDLE;
   VkPhysicalDevice gpu_ = VK_NULL_HANDLE;
   VkDevice device_ = VK_NULL_HANDLE;

   VmaAllocator memory_allocator_ = VK_NULL_HANDLE;

   std::unordered_map<queue::flag, queue> queues_;
   std::unordered_map<std::uint32_t, command_pool> command_pools_;

   VkCommandBuffer transfer_command_buffer = VK_NULL_HANDLE;

   std::vector<vk::layer> validation_layers_;
   std::vector<vk::extension> instance_extensions_;
   std::vector<vk::extension> device_extensions_;
  
   std::shared_ptr<spdlog::logger> validation_layer_logger_;
   std::shared_ptr<spdlog::logger> vulkan_logger_;
};

using p_context_t = strong_type<context const*>;

#endif // LUCIOLE_CONTEXT_HPP
