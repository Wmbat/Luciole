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

#ifndef TWE_VK_PIPELINE_MANAGER_H
#define TWE_VK_PIPELINE_MANAGER_H

#include <unordered_map>

#include "../vk_utils.h"
#include "pipeline.h"
#include "shader_manager.h"

namespace twe
{
    class pipeline_manager
    {
    public:
        struct pipeline_create_info
        {
            shader::id vert_id_;
            shader::id frag_id_;

            pipeline::type type_;
    
            std::vector<std::string> pipeline_defs_;
            
            const shader_manager* shader_manager_;
            
            vk::Device device_;
            vk::RenderPass render_pass_;
            vk::Extent2D extent_;
            
            pipeline_create_info& TWE_API set_shaders( const shader::id vert_shader_id, const shader::id frag_shader_id )
            {
                vert_id_ = vert_shader_id;
                frag_id_ = frag_shader_id;
                
                return *this;
            }
            pipeline_create_info& TWE_API set_pipeline_type( const pipeline::type type )
            {
                type_ = type;
                
                return *this;
            }
            pipeline_create_info& TWE_API set_shader_manager( const shader_manager* manager )
            {
                shader_manager_ = manager;
                
                return *this;
            }
            pipeline_create_info& TWE_API set_pipeline_definitions( const std::vector<std::string> pipeline_defs )
            {
                pipeline_defs_ = pipeline_defs;
                
                return *this;
            }
            pipeline_create_info& TWE_API set_device( const vk::Device& device )
            {
                device_ = device;
                
                return *this;
            }
            pipeline_create_info& TWE_API set_render_pass( const vk::RenderPass& render_pass )
            {
                render_pass_ = render_pass;
                
                return *this;
            }
            pipeline_create_info& TWE_API set_extent( const vk::Extent2D extent )
            {
                extent_ = extent;
                
                return *this;
            }
        };
        
    public:
        std::vector<pipeline::id> TWE_API insert( pipeline_create_info& create_info );
        
        const pipeline& TWE_API find( const pipeline::id id ) const;
        
        const pipeline& TWE_API operator[]( const pipeline::id id ) const;
        
    private:
        const vk::Bool32 get_bool_data( const std::string& data, const std::string& problem_location ) const noexcept;
        
        // Input Assembly //
        const vk::PrimitiveTopology get_topology_data( const std::string& data ) const noexcept;
        
        // Rasterization //
        const vk::PolygonMode get_polygon_mode_data( const std::string& data ) const noexcept;
        const vk::CullModeFlags get_cull_mode_data( const std::string& data ) const noexcept;
        const vk::FrontFace get_front_face_data( const std::string& data ) const noexcept;
        
        // Multisample //
        const vk::SampleCountFlagBits get_rasterization_samples_data( const std::uint32_t data ) const noexcept;
        
        // Colour Blend Attachments //
        const vk::BlendFactor set_blend_factor_data( const std::string& data ) const noexcept;
        const vk::BlendOp set_blend_op_data( const std::string& data ) const noexcept;
        
        // Colour Blend //
        const vk::LogicOp set_logic_op_data( const std::string& data ) const noexcept;
        
    private:
        std::unordered_map<pipeline::id, pipeline> pipelines_;
        std::unordered_map<pipeline_layout::id, pipeline_layout> layouts_;
        
        static pipeline::id pipeline_id_count_;
        static pipeline_layout::id layout_id_count_;
    };
}

#endif //TWE_VK_PIPELINE_MANAGER_H
