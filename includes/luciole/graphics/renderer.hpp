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

#ifndef LUCIOLE_GRAPHICS_RENDERER_HPP
#define LUCIOLE_GRAPHICS_RENDERER_HPP

/* INCLUDES */
#include <luciole/context.hpp>
#include <luciole/sys/component.hpp>
#include <luciole/utils/strong_types.hpp>
#include <luciole/vk/buffers/index_buffer.hpp>
#include <luciole/vk/buffers/uniform_buffer.hpp>
#include <luciole/vk/buffers/vertex_buffer.hpp>
#include <luciole/vk/pipelines/pipeline_manager.hpp>
#include <luciole/vk/shaders/shader_manager.hpp>

#include <vulkan/vulkan.h>

#include <vector>

/**
 * @brief The main class to render object.
 */
class renderer
{
private:
   /**
    * @brief Dummy struct for custom strong type to designate a shader filepath.
    */
   struct shader_filepath_parameter
   {
   };
   using shader_filepath_t = strong_type<std::string const&, shader_filepath_parameter>;

   /**
    * @brief Dummy struct for custom strong type to designate a vertex shader filepath.
    */
   struct vert_shader_filepath_param
   {
   };
   using vert_shader_filepath_t = strong_type<std::string const&, vert_shader_filepath_param>;

   /**
    * @brief Dummy struct for custom strong type to designate a fragment shader filepath.
    */
   struct frag_shader_filepath_param
   {
   };
   using frag_shader_filepath_t = strong_type<std::string const&, frag_shader_filepath_param>;

public:
   renderer( ) = default;
   renderer( p_context_t p_context, ui::window& wnd );
   renderer( renderer const& rhs ) = delete;
   renderer( renderer&& rhs );
   ~renderer( );

   renderer& operator=( renderer const& rhs ) = delete;
   renderer& operator=( renderer&& rhs );

   void draw_frame( );

   void on_framebuffer_resize( framebuffer_resize_event const& event );

   vk::shader::id load_shader_module( vk::shader::loader_ptr_t p_loader, vk::shader::filepath_t const& filepath );

   vk::shader::set::id create_shader_pack( vk::shader::set::create_info_t const& create_info );

   vk::pipeline::id create_pipeline( vk::pipeline::loader_ptr_t p_loader, vk::shader::set::id_t pack_id );

private:
   /**
    * @brief Create all objects related to the swapchain.
    */
   void create_swapchain( );
   /**
    * @brief Destroy all objects related to the swapchain.
    */
   void cleanup_swapchain( );

   /**
    * @brief Record the command buffers.
    */
   void record_command_buffers( );

   /**
    * @brief Create a swapchain object.
    *
    * @param capabilities The capabilities of the Surface.
    * @param format The format of the Surface.
    * @return std::variant<VkSwapchainKHR, vk::error> Type safe union that returns
    * either the created Swapchain handle or an error code.
    */
   [[nodiscard]] std::variant<VkSwapchainKHR, vk::error> create_swapchain(
      VkSurfaceCapabilitiesKHR const& capabilities, VkSurfaceFormatKHR const& format ) const PURE;

   /**
    * @brief Create a image view object.
    *
    * @param image The Image to create the view for.
    * @return std::variant<VkImageView, vk::error> Type safe union that either returns
    * an image view or an error code.
    */
   [[nodiscard]] std::variant<VkImageView, vk::error> create_image_view( vk::image_t image ) const PURE;

   /**
    * @brief Create a render pass object.
    *
    * @return std::variant<VkRenderPass, vk::error> Type safe union that either returns
    * a render pass or an error code.
    */
   [[nodiscard]] std::variant<VkRenderPass, vk::error> create_render_pass( ) const PURE;

   /**
    * @brief Create a shader module object.
    *
    * @param filepath The Path to the SPIR-V binary file
    * @return VkShaderModule The shader module generated from the SPIR-V code.
    */
   [[nodiscard]] VkShaderModule create_shader_module( shader_filepath_t filepath ) const PURE;

   /**
    * @brief Create a default pipeline layout object.
    *
    * @return std::variant<VkPipelineLayout, vk::error> Type safe union that either returns a
    * pipeline layout or an error code.
    */
   [[nodiscard]] std::variant<VkPipelineLayout, vk::error> create_default_pipeline_layout( ) const PURE;

   /**
    * @brief Create a descriptor set layout.
    *
    * @param NONE
    *
    * @return The descriptor set layout or an error code.
    */
   [[nodiscard]] std::variant<VkDescriptorSetLayout, vk::error> create_descriptor_set_layout( ) const PURE;

   /**
    * @brief Create a default pipeline object.
    *
    * @param vert_filepath The path from the executable to the vertex shader SPIR-V file.
    * @param frag_filepath The path from the executable to the fragment shader SPIR-V file.
    * @return std::variant<VkPipeline, vk::error> Type safe union that either returns a
    * default graphics pipeline or an error code.
    */
   [[nodiscard]] std::variant<VkPipeline, vk::error> create_default_pipeline(
      vert_shader_filepath_t vert_filepath, frag_shader_filepath_t frag_filepath ) const PURE;

   /**
    * @brief Create a semaphore object.
    *
    * @return std::variant<VkSemaphore, vk::error> Type safe union that either returns a
    * semaphore or an error code.
    */
   [[nodiscard]] std::variant<VkSemaphore, vk::error> create_semaphore( ) const PURE;

   /**
    * @brief Create a fence object.
    *
    * @return std::variant<VkFence, vk::error> Type safe union that either returns a
    * fence or an error code.
    */
   [[nodiscard]] std::variant<VkFence, vk::error> create_fence( ) const PURE;

   /**
    * @brief Create a framebuffer object.
    *
    * @param image_view
    * @return std::variant<VkFramebuffer, vk::error> Type safe union that either returns a
    * framebuffer or an error code.
    */
   [[nodiscard]] std::variant<VkFramebuffer, vk::error> create_framebuffer( vk::image_view_t image_view ) const PURE;

   /**
    * @brief Pick a surface format for the swapchain.
    *
    * @return VkSurfaceFormatKHR The chosen surface format.
    */
   [[nodiscard]] VkSurfaceFormatKHR pick_swapchain_format( ) const PURE;

   /**
    * @brief Pick a surface present mode for the swapchain.
    *
    * @return VkPresentModeKHR The chosen present mode.
    */
   [[nodiscard]] VkPresentModeKHR pick_swapchain_present_mode( ) const PURE;

   /**
    * @brief Pick a extent supported by the surface for the
    * swapchain.
    *
    * @param [in] capabilities The surface capabilities.
    * @return VkExtent2D The chosen extent.
    */
   [[nodiscard]] VkExtent2D pick_swapchain_extent( VkSurfaceCapabilitiesKHR const& capabilities ) const PURE;

private:
   static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

   const context* p_context;

   VkSwapchainKHR swapchain = VK_NULL_HANDLE;
   std::vector<VkImage> swapchain_images = {};
   std::vector<VkImageView> swapchain_image_views = {};
   std::vector<VkFramebuffer> swapchain_framebuffers = {};

   VkFormat swapchain_image_format;
   VkExtent2D swapchain_extent;

   VkRenderPass render_pass = VK_NULL_HANDLE;
   VkPipeline default_graphics_pipeline = VK_NULL_HANDLE;
   VkPipelineLayout default_graphics_pipeline_layout = VK_NULL_HANDLE;

   VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;

   std::vector<VkCommandBuffer> render_command_buffers = {};

   VkSemaphore image_available_semaphore[MAX_FRAMES_IN_FLIGHT] = {};
   VkSemaphore render_finished_semaphore[MAX_FRAMES_IN_FLIGHT] = {};
   VkFence in_flight_fences[MAX_FRAMES_IN_FLIGHT] = {};

   std::vector<vk::uniform_buffer> uniform_buffers;

   size_t current_frame = 0;

   bool is_framebuffer_resized = false;

   std::uint32_t window_width = 0;
   std::uint32_t window_height = 0;

   vk::vertex_buffer vertex_buffer;
   vk::index_buffer index_buffer;

   vk::shader::manager shader_manager;
   vk::pipeline::manager pipeline_manager;

   std::shared_ptr<spdlog::logger> vulkan_logger;
};

#endif // LUCIOLE_GRAPHICS_RENDERER_HPP
