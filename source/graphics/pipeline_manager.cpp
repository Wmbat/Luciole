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
 
#include <nlohmann/json.hpp>

#include "log.h"
#include "graphics/pipeline_manager.h"

#include "utilities/file_io.h"

namespace twe
{
    pipeline::id pipeline_manager::pipeline_id_count_;
    pipeline_layout::id pipeline_manager::layout_id_count_;
    
    std::vector<pipeline::id> pipeline_manager::insert( pipeline_create_info& create_info )
    {
        auto size = create_info.pipeline_defs_.size();
        
        auto layout_id = ++layout_id_count_;
        layouts_.insert( std::pair{
            layout_id,
            pipeline_layout{ create_info.device_, create_info.vert_id_, create_info.frag_id_ } } );
        
        std::vector<pipeline::id> pipeline_ids_( size );
        std::vector<vk::GraphicsPipelineCreateInfo> create_infos( size );
        
        vk::PipelineInputAssemblyStateCreateInfo input_assembly_create_infos[size];
        vk::Viewport viewports[size];                           // TODO: allow for multiple viewports.
        vk::Rect2D scissors[size];                              // TODO: allow for multiple scissors.
        vk::PipelineViewportStateCreateInfo viewport_create_infos[size];
        vk::PipelineRasterizationStateCreateInfo rasterization_create_infos[size];
        vk::PipelineMultisampleStateCreateInfo multisample_create_infos[size];
        
        std::vector<vk::PipelineColorBlendAttachmentState> attachments[size];
        vk::PipelineColorBlendStateCreateInfo colour_blend_create_infos[size];
        
        std::vector<vk::DynamicState> dynamic_states[size];
        vk::PipelineDynamicStateCreateInfo dynamic_state_create_infos[size];
        
        const vk::PipelineShaderStageCreateInfo shader_stages[] = {
            create_info.shader_manager_->find( create_info.vert_id_ ).get_shader_stage_create_info(),
            create_info.shader_manager_->find( create_info.frag_id_ ).get_shader_stage_create_info() };
    
        const auto vertex_input_create_info = vk::PipelineVertexInputStateCreateInfo( )
            .setVertexAttributeDescriptionCount( 0 )
            .setVertexBindingDescriptionCount( 0 );
        
        for( auto i = 0; i < size; ++i )
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
        
        for( auto i = 0; i < size; ++i )
        {
            pipelines_.insert(
                std::pair{
                    pipeline_ids_[i],
                    pipeline{ std::move( pipelines[i] ), create_info.type_, layout_id }
                } );
        }
        
        return pipeline_ids_;
    }
    const pipeline& pipeline_manager::find( const pipeline::id id ) const
    {
        try
        {
            const auto it = pipelines_.find( id );
            if( it != pipelines_.cend() )
            {
                return it->second;
            }
            else
            {
                throw basic_error{
                    basic_error::code::pipeline_not_present_error,
                    "Pipeline: " + std::to_string( id ) + " is not in the manager, "
                    "Please call pipeline_manager::insert first."
                };
            }
        }
        catch( const basic_error& e )
        {
            // TODO: handle properly.
        }
    }
    const pipeline& pipeline_manager::operator[]( const pipeline::id id ) const
    {
        try
        {
            const auto it = pipelines_.find( id );
            if( it != pipelines_.cend() )
            {
                return it->second;
            }
            else
            {
                throw basic_error{
                    basic_error::code::pipeline_not_present_error,
                    "Pipeline: " + std::to_string( id ) + " is not in the manager, "
                                                          "Please call pipeline_manager::insert first."
                };
            }
        }
        catch( const basic_error& e )
        {
            // TODO: handle properly.
        }
    }
    
    const vk::Bool32
    pipeline_manager::get_bool_data( const std::string& data, const std::string& problem_location ) const noexcept
    {
        if( data == "false" )
        {
            return VK_FALSE;
        }
        else if ( data == "true" )
        {
            return VK_TRUE;
        }
        else
        {
            core_warn( problem_location +
                       "attribute of pipeline #{0} is invalid, check json file.", pipeline_id_count_ );
            
            return VK_FALSE;
        }
    }
    
    // Input Assembly //
    const vk::PrimitiveTopology pipeline_manager::get_topology_data( const std::string& data ) const noexcept
    {
        if ( data == "triangle_list" )
        {
            return vk::PrimitiveTopology::eTriangleList;
        }
        else if ( data == "triangle_strip" )
        {
            return vk::PrimitiveTopology::eTriangleStrip;
        }
        else if ( data == "triangle_fan" )
        {
            return vk::PrimitiveTopology::eTriangleFan;
        }
        else if ( data == "triangle_list_with_adjacency" )
        {
            return vk::PrimitiveTopology::eTriangleListWithAdjacency;
        }
        else if ( data == "triangle_strip_with_adjacency" )
        {
            return vk::PrimitiveTopology::eTriangleStripWithAdjacency;
        }
        else if ( data == "line_list" )
        {
            return vk::PrimitiveTopology::eLineList;
        }
        else if ( data == "line_strip" )
        {
            return vk::PrimitiveTopology::eLineStrip;
        }
        else if ( data == "line_list_with_adjacency" )
        {
            return vk::PrimitiveTopology::eLineListWithAdjacency;
        }
        else if ( data == "line_strip_with_adjacency" )
        {
            return vk::PrimitiveTopology::eLineStripWithAdjacency;
        }
        else if ( data == "patch_list" )
        {
            return vk::PrimitiveTopology::ePatchList;
        }
        else if ( data == "point_list" )
        {
            return vk::PrimitiveTopology::ePointList;
        }
        else
        {
            core_warn( "\"input_assembly\"/\"topology\" "
                       "attribute of pipeline #{0} is incorrect, check json file.", pipeline_id_count_ );
        
            return vk::PrimitiveTopology::eTriangleList;
        }
    }
    
    // Rasterization //
    const vk::PolygonMode pipeline_manager::get_polygon_mode_data( const std::string& data ) const noexcept
    {
        if( data == "point" )
        {
            return vk::PolygonMode::ePoint;
        }
        else if( data == "line" )
        {
            return vk::PolygonMode::eLine;
        }
        else if( data == "fill" )
        {
            return vk::PolygonMode::eFill;
        }
        else
        {
            core_warn( "\"rasterization\"/\"polygon_mode\" "
                       "attribute of pipeline #{0} is invalid, check json file.", pipeline_id_count_ );
            
            return vk::PolygonMode::eFill;
        }
    }
    const vk::CullModeFlags pipeline_manager::get_cull_mode_data( const std::string& data ) const noexcept
    {
        if( data == "none" )
        {
            return vk::CullModeFlagBits::eNone;
        }
        else if( data == "front" )
        {
            return vk::CullModeFlagBits::eFront;
        }
        else if ( data == "back" )
        {
            return vk::CullModeFlagBits::eBack;
        }
        else if ( data == "front_and_back" )
        {
            return vk::CullModeFlagBits::eFrontAndBack;
        }
        else
        {
            core_warn( "\"rasterization\"/\"cull_mode\" "
                       "attribute of pipeline #{0} is invalid, check json file.", pipeline_id_count_ );
            
            return vk::CullModeFlagBits::eNone;
        }
    }
    const vk::FrontFace pipeline_manager::get_front_face_data( const std::string& data ) const noexcept
    {
        if ( data == "clockwise" )
        {
            return vk::FrontFace::eClockwise;
        }
        else if ( data == "counter_clockwise" )
        {
            return vk::FrontFace::eCounterClockwise;
        }
        else
        {
            core_warn( "\"rasterization\"/\"front_face\" "
                       "attribute of pipeline #{0} is invalid, check json file.", pipeline_id_count_ );
            
            return vk::FrontFace::eCounterClockwise;
        }
    }
    
    // Multisample //
    const vk::SampleCountFlagBits pipeline_manager::get_rasterization_samples_data( const std::uint32_t data ) const noexcept
    {
        if ( data == 1 )
        {
            return vk::SampleCountFlagBits::e1;
        }
        else if ( data == 2 )
        {
            return vk::SampleCountFlagBits::e2;
        }
        else if ( data == 4 )
        {
            return vk::SampleCountFlagBits::e4;
        }
        else if ( data == 8 )
        {
            return vk::SampleCountFlagBits::e8;
        }
        else if ( data == 16 )
        {
            return vk::SampleCountFlagBits::e16;
        }
        else if ( data == 32 )
        {
            return vk::SampleCountFlagBits::e32;
        }
        else if ( data == 64 )
        {
            return vk::SampleCountFlagBits::e64;
        }
        else
        {
            core_warn( "\"multisample\"/\"rasterization_samples\" "
                       "attribute of pipeline #{0} is invalid, check json file.", pipeline_id_count_ );
            
            return vk::SampleCountFlagBits::e1;
        }
    }
    
    // Colour Blend Attachments //
    const vk::BlendFactor pipeline_manager::set_blend_factor_data( const std::string& data ) const noexcept
    {
        return vk::BlendFactor::eOne;
    }
    const vk::BlendOp pipeline_manager::set_blend_op_data( const std::string& data ) const noexcept
    {
        return vk::BlendOp::eAdd;
    }
    
    // Colour Blend //
    const vk::LogicOp pipeline_manager::set_logic_op_data( const std::string& data ) const noexcept
    {
        if( data == "copy" )
        {
            return vk::LogicOp::eCopy;
        }
        else
        {
            // TODO: handle all other cases.
        }
    }
}
