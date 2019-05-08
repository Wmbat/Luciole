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

#include <optional>

#include <tiny_gltf/tiny_gltf.h>
#include <wmbats_bazaar/delegate.hpp>

#include "../luciole_core.hpp"
#include "../instance.hpp"
#include "../surface.hpp"
#include "../device.hpp"

#include "../window/base_window.hpp"

#include "../vulkan/context.hpp"
#include "../vulkan/swapchain.hpp"
#include "../vulkan/memory_allocator.hpp"
#include "../vulkan/shader_manager.hpp"
#include "../vulkan/pipeline_manager.hpp"
#include "../vulkan/mesh_buffer.hpp"

namespace lcl
{
    enum class pipeline_type
    {
        graphics,
        compute
    };
 

    class renderer
    {
    public:
        LUCIOLE_API renderer( base_window* p_window, const std::string& app_name, uint32_t app_version );
        LUCIOLE_API renderer( const renderer& renderer ) noexcept = delete;
        LUCIOLE_API renderer( renderer&& renderer ) noexcept;
        LUCIOLE_API ~renderer( );

        LUCIOLE_API renderer& operator=( const renderer& renderer ) noexcept = delete;
        LUCIOLE_API renderer& operator=( renderer&& renderer ) noexcept;
        
        /*!
         * @brief Creates a vertex shader and place it in the shader manager.
         * 
         * @param the filepath of the shader.
         * @param the name of the main function of the shader.
         * @return an index to allow the programmer access to the shader.
         */
        LUCIOLE_API std::uint32_t create_vertex_shader( const std::string_view filepath, const std::string_view entry_point = "main" ); 
        /*!
         * @brief Creates a fragment shader and place it in the shader manager.
         * 
         * @param the filepath of the shader.
         * @param the name of the main function of the shader.
         * @return an index to allow the programmer access to the shader.
         */
        LUCIOLE_API std::uint32_t create_fragment_shader( const std::string_view filepath, const std::string_view entry_point = "main" );        

        /*!
         * @brief Creates a graphics pipeline.
         * 
         * @param the filepath to the json file holding the pipeline information.
         * @param the id of the vertex shader to use in this pipeline.
         * @param the id of the fragment shader to use in this pipeline.
         * @return the id of the pipeline that was just created.
         */
        LUCIOLE_API std::uint32_t create_graphics_pipeline( const std::string_view filepath, std::uint32_t vertex_shader_id, std::uint32_t fragment_shader_id );
        
        LUCIOLE_API void set_pipeline( const uint32_t id );
        LUCIOLE_API void switch_pipeline( const uint32_t id );

        LUCIOLE_API void draw_frame( );

        LUCIOLE_API void record_draw_calls( );

        LUCIOLE_API void on_window_close( const window_close_event& event );
        LUCIOLE_API void on_framebuffer_resize( const framebuffer_resize_event& event );

        LUCIOLE_API void set_clear_colour( const glm::vec4& colour );
    
    private:
        const vk::UniqueSemaphore create_semaphore( ) const noexcept;

        const vk::UniqueFence create_fence( ) const noexcept;
        
        const std::vector<vk::UniqueCommandBuffer> create_command_buffers( 
            const vk::CommandPool command_pool, 
            std::uint32_t count ) const;
        
        const vk::UniqueRenderPass create_render_pass( 
            const vk::SurfaceFormatKHR surface_format,
            const vk::PipelineBindPoint bind_point = vk::PipelineBindPoint::eGraphics ) const noexcept;
    
        const vk::SurfaceFormatKHR choose_swapchain_surface_format(
            const std::vector<vk::SurfaceFormatKHR> &available_formats ) const noexcept;
        
    private:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        
        uint32_t window_width_;
        uint32_t window_height_;

        bool is_window_closed_ = false;
        bool framebuffer_resized_ = false;
    
        glm::vec4 clear_colour_;
        
        size_t current_frame_ = 0;
        
        uint32_t current_pipeline_;

        vulkan::context context_;
        vulkan::swapchain swapchain_;
    
        std::vector<vk::UniqueSemaphore> image_available_semaphores_;
        std::vector<vk::UniqueSemaphore> render_finished_semaphores_;
        std::vector<vk::UniqueFence> in_flight_fences_;
    
        std::vector<vk::UniqueCommandBuffer> render_command_buffers_[MAX_FRAMES_IN_FLIGHT];
        
        std::vector<vk::UniqueCommandBuffer> transfer_command_buffers_;
    
        vk::UniqueRenderPass render_pass_;
        
        vulkan::memory_allocator memory_allocator_;
        
        vulkan::mesh_buffer test_mesh_buffer_;
        
        vulkan::shader_manager shader_manager_;
        vulkan::pipeline_manager pipeline_manager_;

        vulkan::instance inst_;
        vulkan::surface surface_;
        vulkan::device device_;
    };
}

#endif // LUCIOLE_GRAPHICS_RENDERER_HPP