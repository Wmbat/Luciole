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

#include <nlohmann/json.hpp>

#include "vk_utils.h"
#include "pipeline.h"
#include "graphics/shader_manager.h"

namespace TWE
{
    class vk_pipeline_manager
    {
    public:
        struct pipeline_create_info
        {
            shader::id vert_id_ = 0;
            shader::id frag_id_ = 0;
            
            shader_manager& shader_manager_;
            
            std::vector<std::string> pipeline_def_filepaths;
        
            pipeline::type type_;
            
            vk::Device& device_;
            vk::RenderPass& render_pass_;
            vk::Extent2D extent_;
        };
        
    public:
        std::vector<pipeline::id> insert( const pipeline_create_info& create_info );
        
        const pipeline& get_pipeline( const pipeline::id id );
        
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
        const vk::BlendFactor get_src_colour_blend_factor( const std::string& data ) const noexcept;
        
        vk::PipelineMultisampleStateCreateInfo set_multisample_create_info( const nlohmann::json& json );
        std::vector<vk::PipelineColorBlendAttachmentState> set_colour_blend_attachment( const nlohmann::json& json );
        vk::PipelineColorBlendStateCreateInfo set_colour_blend_create_info( const nlohmann::json& json,
            const std::vector<vk::PipelineColorBlendAttachmentState>& attachments );
        
    private:
        std::unordered_map<pipeline::id, pipeline> pipelines_;
        std::unordered_map<pipeline_layout::id, pipeline_layout> layouts_;
        
        static pipeline::id pipeline_id_count_;
        static pipeline_layout::id layout_id_count_;
    };
}

#endif //TWE_VK_PIPELINE_MANAGER_H
