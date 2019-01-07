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

#include <nlohmann/json.hpp>

#include <unordered_map>

#include "pipeline.hpp"
#include "shader_manager.hpp"
#include "../twe_core.hpp"
#include "../utilities/file_io.hpp"

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
            
            pipeline_create_info& set_shaders( const shader::id vert_shader_id, const shader::id frag_shader_id )
            {
                vert_id_ = vert_shader_id;
                frag_id_ = frag_shader_id;
                
                return *this;
            }
            pipeline_create_info& set_pipeline_type( const pipeline::type type )
            {
                type_ = type;
                
                return *this;
            }
            pipeline_create_info& set_shader_manager( const shader_manager* manager )
            {
                shader_manager_ = manager;
                
                return *this;
            }
            pipeline_create_info& set_pipeline_definitions( const std::vector<std::string> pipeline_defs )
            {
                pipeline_defs_ = pipeline_defs;
                
                return *this;
            }
            pipeline_create_info& set_device( const vk::Device& device )
            {
                device_ = device;
                
                return *this;
            }
            pipeline_create_info& set_render_pass( const vk::RenderPass& render_pass )
            {
                render_pass_ = render_pass;
                
                return *this;
            }
            pipeline_create_info& set_extent( const vk::Extent2D extent )
            {
                extent_ = extent;
                
                return *this;
            }
        };
        
    public:
        template<size_t count>
        std::vector<pipeline::id> insert( pipeline_create_info& create_info )
        {
            auto layout_id = ++layout_id_count_;
            layouts_.insert( std::pair{
                layout_id,
                pipeline_layout{ create_info.device_, create_info.vert_id_, create_info.frag_id_ } } );
    
            std::vector<pipeline::id> pipeline_ids_( count );
            std::vector<vk::GraphicsPipelineCreateInfo> create_infos( count );
    
            vk::PipelineInputAssemblyStateCreateInfo input_assembly_create_infos[count];
            vk::Viewport viewports[count];                           // TODO: allow for multiple viewports.
            vk::Rect2D scissors[count];                              // TODO: allow for multiple scissors.
            vk::PipelineViewportStateCreateInfo viewport_create_infos[count];
            vk::PipelineRasterizationStateCreateInfo rasterization_create_infos[count];
            vk::PipelineMultisampleStateCreateInfo multisample_create_infos[count];
    
            std::vector<vk::PipelineColorBlendAttachmentState> attachments[count];
            vk::PipelineColorBlendStateCreateInfo colour_blend_create_infos[count];
    
            std::vector<vk::DynamicState> dynamic_states[count];
            vk::PipelineDynamicStateCreateInfo dynamic_state_create_infos[count];
    
            const vk::PipelineShaderStageCreateInfo shader_stages[] = {
                create_info.shader_manager_->find( create_info.vert_id_ ).get_shader_stage_create_info(),
                create_info.shader_manager_->find( create_info.frag_id_ ).get_shader_stage_create_info() };
    
            const auto vertex_input_create_info = vk::PipelineVertexInputStateCreateInfo( )
                .setVertexAttributeDescriptionCount( 0 )
                .setVertexBindingDescriptionCount( 0 );
    
            for( auto i = 0; i < count; ++i )
            {
                auto id = ++pipeline_id_count_;
                pipeline_ids_[i] = id;
        
                auto json = nlohmann::json::parse( read_from_file( create_info.pipeline_defs_[i] ) );
        
                input_assembly_create_infos[i].topology = get_topology_data( json["input_assembly"]["topology"] );
                input_assembly_create_infos[i].primitiveRestartEnable = get_bool_data(
                    json["input_assembly"]["primitive_restart_enable"],
                    "\"input_assembly\"/\"primitive_restart_enable\" " );
        
                viewports[i].x = 0.0f;
                viewports[i].y = 0.0f;
                viewports[i].width = static_cast<float>( create_info.extent_.width );
                viewports[i].height = static_cast<float>( create_info.extent_.height );
                viewports[i].minDepth = 0.0f;
                viewports[i].maxDepth = 1.0f;
        
                scissors[i].offset = vk::Offset2D{ 0, 0 };
                scissors[i].extent = create_info.extent_;
        
                viewport_create_infos[i].viewportCount = 1;
                viewport_create_infos[i].pViewports = &viewports[i];
                viewport_create_infos[i].scissorCount = 1;
                viewport_create_infos[i].pScissors = &scissors[i];
        
                /// Rasterization ///
                rasterization_create_infos[i].depthClampEnable = get_bool_data(
                    json["rasterization"]["depth_clamp_enable"],
                    "\"rasterization\"/\"depth_clamp_enable\" " );
        
                rasterization_create_infos[i].rasterizerDiscardEnable = get_bool_data(
                    json["rasterization"]["rasterization_discard_enable"],
                    "\"rasterization\"/\"rasterizer_discard_enable\" " );
        
                rasterization_create_infos[i].depthBiasEnable = get_bool_data(
                    json["rasterization"]["depth_bias_enable"],
                    "\"rasterization\"/\"depth_bias_enable\" " );
        
                rasterization_create_infos[i].polygonMode = get_polygon_mode_data( json["rasterization"]["polygon_mode"] );
                rasterization_create_infos[i].cullMode = get_cull_mode_data( json["rasterization"]["cull_mode"] );
                rasterization_create_infos[i].frontFace = get_front_face_data( json["rasterization"]["front_face"] );
                rasterization_create_infos[i].depthBiasConstantFactor = json["rasterization"]["depth_bias_constant_factor"];
                rasterization_create_infos[i].depthBiasClamp = json["rasterization"]["depth_bias_clamp"];
                rasterization_create_infos[i].depthBiasSlopeFactor = json["rasterization"]["depth_bias_slope_factor"];
                rasterization_create_infos[i].lineWidth = json["rasterization"]["line_width"];
        
                /// Multisample ///
                multisample_create_infos[i].rasterizationSamples = get_rasterization_samples_data(
                    json["multisample"]["rasterization_samples"] );
        
                multisample_create_infos[i].sampleShadingEnable = get_bool_data(
                    json["multisample"]["sample_shading_enable"],
                    "\"multisample\"/\"sample_shading_enable_data\" " );
        
                multisample_create_infos[i].alphaToCoverageEnable = get_bool_data(
                    json["multisample"]["alpha_to_coverage_enable"],
                    "\"multisample\"/\"alpha_to_coverage_enable\" " );
        
                multisample_create_infos[i].alphaToOneEnable = get_bool_data(
                    json["multisample"]["alpha_to_one_enable"],
                    "\"multisample\"/\"alpha_to_one_enable\" ");
        
                multisample_create_infos[i].minSampleShading = json["multisample"]["min_sample_shading"];
                multisample_create_infos[i].pSampleMask = nullptr;
        
        
                std::vector<nlohmann::json> attachments_data = json["colour_blend_attachments"];
                for( const auto & data : attachments_data )
                {
                    const auto colour_blend_attachment = vk::PipelineColorBlendAttachmentState( )
                        .setBlendEnable( VK_FALSE )
                        .setSrcColorBlendFactor( vk::BlendFactor::eOne )
                        .setDstColorBlendFactor( vk::BlendFactor::eZero )
                        .setColorBlendOp( vk::BlendOp::eAdd )
                        .setSrcAlphaBlendFactor( vk::BlendFactor::eOne )
                        .setDstAlphaBlendFactor( vk::BlendFactor::eZero )
                        .setAlphaBlendOp( vk::BlendOp::eAdd )
                        .setColorWriteMask( vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA );
            
                    attachments[i].emplace_back( colour_blend_attachment );
                }
        
                /// Colour Blend ///
                colour_blend_create_infos[i].logicOpEnable = get_bool_data(
                    json["colour_blend"]["logic_op_enable"],
                    "\"colour_blend\"/\"logic_op_enable\" " );
        
                colour_blend_create_infos[i].logicOp = set_logic_op_data( json["colour_blend"]["logic_op"] );
        
                colour_blend_create_infos[i].attachmentCount = static_cast<uint32_t>( attachments[i].size() );
                colour_blend_create_infos[i].pAttachments = attachments[i].data();
        
                colour_blend_create_infos[i].blendConstants[0] = json["colour_blend"]["blend_constants"][0];
                colour_blend_create_infos[i].blendConstants[1] = json["colour_blend"]["blend_constants"][1];
                colour_blend_create_infos[i].blendConstants[2] = json["colour_blend"]["blend_constants"][2];
                colour_blend_create_infos[i].blendConstants[3] = json["colour_blend"]["blend_constants"][3];
        
                /// Dynamic States ///
                std::vector<std::string> dynamic_state_names = json["dynamic_states"];;
                for( const auto& state : dynamic_state_names )
                {
                    if( state == "viewport" )
                    {
                        dynamic_states[i].emplace_back( vk::DynamicState::eViewport );
                    }
                    else if( state == "scissors" )
                    {
                        dynamic_states[i].emplace_back( vk::DynamicState::eScissor );
                    }
                }
        
                dynamic_state_create_infos[i].dynamicStateCount = static_cast<uint32_t>( dynamic_states[i].size() );
                dynamic_state_create_infos[i].pDynamicStates = dynamic_states[i].data();
        
                /// Pipeline create info ///
                create_infos[i] = vk::GraphicsPipelineCreateInfo( )
                    .setLayout( layouts_.at( layout_id ).get() )
                    .setRenderPass( create_info.render_pass_ )
                    .setStageCount( sizeof( shader_stages ) / sizeof( shader_stages[0] ) )
                    .setPStages( shader_stages )
                    .setPVertexInputState( &vertex_input_create_info )
                    .setPInputAssemblyState( &input_assembly_create_infos[i] )
                    .setPViewportState( &viewport_create_infos[i] )
                    .setPRasterizationState( &rasterization_create_infos[i] )
                    .setPMultisampleState( &multisample_create_infos[i] )
                    .setPColorBlendState( &colour_blend_create_infos[i] )
                    .setPDynamicState( &dynamic_state_create_infos[i] )
                    .setBasePipelineIndex( -1 );
            }
    
            auto pipelines = create_info.device_.createGraphicsPipelinesUnique( nullptr, create_infos );
    
            for( auto i = 0; i < count; ++i )
            {
                pipelines_.insert(
                    std::pair{
                        pipeline_ids_[i],
                        pipeline{ std::move( pipelines[i] ), create_info.type_, layout_id }
                    } );
            }
    
            return pipeline_ids_;
        }
        
        TWE_API const pipeline& find( const pipeline::id id ) const;
        
        TWE_API const pipeline& operator[]( const pipeline::id id ) const;
        
    private:
        TWE_API const vk::Bool32 get_bool_data( const std::string& data, const std::string& problem_location ) const noexcept;
        
        // Input Assembly //
        TWE_API const vk::PrimitiveTopology get_topology_data( const std::string& data ) const noexcept;
        
        // Rasterization //
        TWE_API const vk::PolygonMode get_polygon_mode_data( const std::string& data ) const noexcept;
        TWE_API const vk::CullModeFlags get_cull_mode_data( const std::string& data ) const noexcept;
        TWE_API const vk::FrontFace get_front_face_data( const std::string& data ) const noexcept;
        
        // Multisample //
        TWE_API const vk::SampleCountFlagBits get_rasterization_samples_data( const std::uint32_t data ) const noexcept;
        
        // Colour Blend Attachments //
        TWE_API const vk::BlendFactor set_blend_factor_data( const std::string& data ) const noexcept;
        TWE_API const vk::BlendOp set_blend_op_data( const std::string& data ) const noexcept;
        
        // Colour Blend //
        TWE_API const vk::LogicOp set_logic_op_data( const std::string& data ) const noexcept;
        
    private:
        std::unordered_map<pipeline::id, pipeline> pipelines_;
        std::unordered_map<pipeline_layout::id, pipeline_layout> layouts_;
        
        static inline pipeline::id pipeline_id_count_;
        static inline pipeline_layout::id layout_id_count_;
    };
}

#endif //TWE_VK_PIPELINE_MANAGER_H
