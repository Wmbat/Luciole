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
 
#include "log.h"
#include "vk_pipeline_manager.h"

#include "utilities/file_io.h"

namespace TWE
{
    pipeline::id vk_pipeline_manager::pipeline_id_count_;
    pipeline_layout::id vk_pipeline_manager::layout_id_count_;
    
    std::vector<pipeline::id> vk_pipeline_manager::insert( const pipeline_create_info& create_info )
    {
        auto size = create_info.pipeline_def_filepaths.size();
        
        std::vector<pipeline::id> pipeline_ids_( size );
        std::vector<vk::GraphicsPipelineCreateInfo> create_infos( size );
    
        auto layout_id = ++layout_id_count_;
        layouts_.insert( std::pair{ layout_id, pipeline_layout{ create_info.device_ } } );
    
        const vk::PipelineShaderStageCreateInfo shader_stages[] = {
            create_info.shader_manager_.acquire( create_info.vert_id_ ).get_shader_stage_create_info(),
            create_info.shader_manager_.acquire( create_info.frag_id_ ).get_shader_stage_create_info() };
    
        const auto vertex_input_create_info = vk::PipelineVertexInputStateCreateInfo( )
            .setVertexAttributeDescriptionCount( 0 )
            .setVertexBindingDescriptionCount( 0 );
        
        for( auto i = 0; i < size; ++i )
        {
            auto id = ++pipeline_id_count_;
            pipeline_ids_[i] = id;
    
            auto pipeline_json = nlohmann::json::parse( read_from_file( create_info.pipeline_def_filepaths[i] ) );
    
            const auto input_assembly_state_create_info = vk::PipelineInputAssemblyStateCreateInfo( )
                .setTopology(
                    get_topology_data( pipeline_json["input_assembly"]["topology"] ) )
                .setPrimitiveRestartEnable(
                    get_bool_data(
                        pipeline_json["input_assembly"]["primitive_restart_enable"],
                        "\"input_assembly\"/\"primitive_restart_enable\" " ) );
    
            const auto viewport = vk::Viewport( )
                .setX( 0.0f )
                .setY( 0.0f )
                .setWidth( static_cast<float>( create_info.extent_.width ) )
                .setHeight( static_cast<float>( create_info.extent_.height ) )
                .setMinDepth( 0.0f )
                .setMaxDepth( 1.0f );
    
            const auto scissors = vk::Rect2D( )
                .setOffset( { 0, 0 } )
                .setExtent( create_info.extent_ );
    
            const auto viewport_state_create_info = vk::PipelineViewportStateCreateInfo( )
                .setViewportCount( 1 )
                .setPViewports( &viewport )
                .setScissorCount( 1 )
                .setPScissors( &scissors );
    
            const auto rasterization_state_create_info = vk::PipelineRasterizationStateCreateInfo( )
                .setDepthClampEnable(
                    get_bool_data(
                        pipeline_json["rasterization"]["depth_clamp_enable"],
                        "\"rasterization\"/\"depth_clamp_enable\" " ) )
                .setRasterizerDiscardEnable(
                    get_bool_data(
                        pipeline_json["rasterization"]["rasterization_discard_enable"],
                        "\"rasterization\"/\"rasterizer_discard_enable\" " ) )
                .setPolygonMode(
                    get_polygon_mode_data( pipeline_json["rasterization"]["polygon_mode"] ) )
                .setCullMode(
                    get_cull_mode_data( pipeline_json["rasterization"]["cull_mode"] ) )
                .setFrontFace(
                    get_front_face_data( pipeline_json["rasterization"]["front_face"] ) )
                .setDepthBiasEnable(
                    get_bool_data(
                        pipeline_json["rasterization"]["depth_bias_enable"],
                        "\"rasterization\"/\"depth_bias_enable\" " ) )
                .setDepthBiasConstantFactor( pipeline_json["rasterization"]["depth_bias_constant_factor"] )
                .setDepthBiasClamp( pipeline_json["rasterization"]["depth_bias_clamp"] )
                .setDepthBiasSlopeFactor( pipeline_json["rasterization"]["depth_bias_slope_factor"] )
                .setLineWidth( pipeline_json["rasterization"]["line_width"] );
    
            const auto multisample_state_create_info = vk::PipelineMultisampleStateCreateInfo( )
                .setRasterizationSamples(
                    get_rasterization_samples_data( pipeline_json["multisample"]["rasterization_samples"] ) )
                .setSampleShadingEnable(
                    get_bool_data(
                        pipeline_json["multisample"]["sample_shading_enable"],
                        "\"multisample\"/\"sample_shading_enable_data\" " ) )
                .setMinSampleShading( pipeline_json["multisample"]["min_sample_shading"] )
                .setPSampleMask( nullptr )
                .setAlphaToCoverageEnable(
                    get_bool_data(
                        pipeline_json["multisample"]["alpha_to_coverage_enable"],
                        "\"multisample\"/\"alpha_to_coverage_enable\" " ) )
                .setAlphaToOneEnable(
                    get_bool_data(
                        pipeline_json["multisample"]["alpha_to_one_enable"],
                        "\"multisample\"/\"alpha_to_one_enable\" ") );
    
            std::vector<nlohmann::json> attachments_data = pipeline_json["colour_blend_attachments"];
            std::vector<vk::PipelineColorBlendAttachmentState> colour_blend_attachments;
            for( const auto& data : attachments_data )
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
        
                colour_blend_attachments.push_back( colour_blend_attachment );
            }
    
            const auto colour_blend_state_create_info = vk::PipelineColorBlendStateCreateInfo( )
                .setLogicOpEnable( VK_FALSE )
                .setLogicOp( vk::LogicOp::eCopy )
                .setAttachmentCount( colour_blend_attachments.size() )
                .setPAttachments( colour_blend_attachments.data() )
                .setBlendConstants( { 0.0f, 0.0f, 0.0f, 0.0f } );
    
            std::vector<std::string> dynamic_state_names = pipeline_json["dynamic_states"];
            std::vector<vk::DynamicState> dynamic_states;
    
            for( const auto& state : dynamic_state_names )
            {
                if( state == "viewport" )
                {
                    dynamic_states.emplace_back( vk::DynamicState::eViewport );
                }
                else if( state == "scissors" )
                {
                    dynamic_states.emplace_back( vk::DynamicState::eScissor );
                }
            }
    
            const auto dynamic_state_create_info = vk::PipelineDynamicStateCreateInfo( )
                .setDynamicStateCount( static_cast<uint32_t>( dynamic_states.size() ) )
                .setPDynamicStates( dynamic_states.data() );
    
            create_infos[i] = vk::GraphicsPipelineCreateInfo( )
                .setLayout( layouts_.at( layout_id ).get() )
                .setRenderPass( create_info.render_pass_ )
                .setStageCount( sizeof( shader_stages ) / sizeof( shader_stages[0] ) )
                .setPStages( shader_stages )
                .setPVertexInputState( &vertex_input_create_info )
                .setPInputAssemblyState( &input_assembly_state_create_info )
                .setPViewportState( &viewport_state_create_info )
                .setPRasterizationState( &rasterization_state_create_info )
                .setPMultisampleState( &multisample_state_create_info )
                .setPColorBlendState( &colour_blend_state_create_info )
                .setPDynamicState( &dynamic_state_create_info )
                .setBasePipelineIndex( -1 );
        }
      
        auto res = create_info.device_.createGraphicsPipelines( nullptr, create_infos );
        check_vk_result( res.result, "failed to create graphics pipelines" );
        
        for( auto i = 0; i < size; ++i )
        {
            pipelines_.insert(
                std::pair{
                    pipeline_ids_[i],
                    pipeline{ &create_info.device_, std::move( res.value[i] ), pipeline::type::graphics, layout_id }
                } );
        }
        
        return pipeline_ids_;
    }
    const pipeline& vk_pipeline_manager::get_pipeline( const pipeline::id id )
    {
        return pipelines_.at( id );
    }
    
    std::vector<vk::PipelineColorBlendAttachmentState> vk_pipeline_manager::set_colour_blend_attachment( const nlohmann::json& json )
    {
        std::vector<nlohmann::json> attachments = json;
        std::vector<vk::PipelineColorBlendAttachmentState> create_infos;
        
        for( const auto& it : attachments )
        {
            auto create_info = vk::PipelineColorBlendAttachmentState( );
            
            if( it["blend_enable"] == "false" )
            {
                create_info.blendEnable = VK_FALSE;
            }
            else if( it["blend_enable"] == "true" )
            {
                create_info.blendEnable = VK_TRUE;
            }
            else
            {
                core_warn( "\"colour_blend_attachments\"/\"blend_enable\" "
                           "attribute of pipeline #{0} is invalid, check json file.", pipeline_id_count_ );
            }
    
            if( it["src_colour_blend_factor"] == "one" )
            {
                create_info.srcColorBlendFactor = vk::BlendFactor::eOne;
            }
            else if ( it["src_colour_blend_factor"] == "zero" )
            {
                create_info.srcColorBlendFactor = vk::BlendFactor::eZero;
            }
            // TODO: handle all cases.
    
            if( it["dst_colour_blend_factor"] == "zero" )
            {
                create_info.dstColorBlendFactor = vk::BlendFactor::eZero;
            }
            else if( it["dst_colour_blend_factor"] == "one" )
            {
                create_info.dstColorBlendFactor = vk::BlendFactor::eOne;
            }
            // TODO: handle all cases.
    
            if( it["colour_blend_op"] == "add" )
            {
                create_info.colorBlendOp = vk::BlendOp::eAdd;
            }
            // TODO: handle all cases.
    
            if( it["src_alpha_blend_factor"] == "one" )
            {
                create_info.srcAlphaBlendFactor = vk::BlendFactor::eOne;
            }
            else if ( it["src_alpha_blend_factor"] == "zero" )
            {
                create_info.srcAlphaBlendFactor = vk::BlendFactor::eZero;
            }
            // TODO: handle all cases.
    
            if( it["dst_alpha_blend_factor"] == "zero" )
            {
                create_info.dstAlphaBlendFactor = vk::BlendFactor::eZero;
            }
            else if( it["dst_alpha_blend_factor"] == "one" )
            {
                create_info.dstAlphaBlendFactor = vk::BlendFactor::eOne;
            }
            // TODO: handle all cases.
    
            if( it["alpha_blend_op"] == "add" )
            {
                create_info.alphaBlendOp = vk::BlendOp::eAdd;
            }
            // TODO: handle all cases.
    
            auto flag_bits = vk::ColorComponentFlags{ };
            const std::vector<std::string> colour_write_mask_names = it["colour_write_mask"];
            for( const auto& name : colour_write_mask_names )
            {
                if( name == "R" || name == "r" )
                {
                    flag_bits |= vk::ColorComponentFlagBits::eR;
                }
                else if( name == "G" || name == "g" )
                {
                    flag_bits |= vk::ColorComponentFlagBits::eR;
                }
                else if( name == "B" || name == "b" )
                {
                    flag_bits |= vk::ColorComponentFlagBits::eB;
                }
                else if( name == "A" || name == "a" )
                {
                    flag_bits |= vk::ColorComponentFlagBits::eA;
                }
                else
                {
                    core_warn( "\"colour_blend_attachments\"/\"colour_write_mask\" "
                               "attribute of pipeline #{0} is invalid, check json file.", pipeline_id_count_ );
                }
            }
            
            create_infos.push_back( create_info );
        }
        
        return create_infos;
    }
    
    vk::PipelineColorBlendStateCreateInfo vk_pipeline_manager::set_colour_blend_create_info(
        const nlohmann::json& json,
        const std::vector<vk::PipelineColorBlendAttachmentState>& attachments )
    {
        auto create_info = vk::PipelineColorBlendStateCreateInfo( )
            .setAttachmentCount( attachments.size() )
            .setPAttachments( attachments.data() );
        
        if( json["logic_op_enable"] == "false" )
        {
            create_info.logicOpEnable = VK_FALSE;
        }
        else if( json["logic_op_enable"] == "true" )
        {
            create_info.logicOpEnable = VK_TRUE;
        }
        else
        {
        
        }
        
        if( json["logic_op"] == "copy" )
        {
            create_info.logicOp = vk::LogicOp::eCopy;
        }
        else
        {
            // TODO: handle all other cases.
        }
        
        create_info.blendConstants[0] = json["blend_constants"][0];
        create_info.blendConstants[1] = json["blend_constants"][1];
        create_info.blendConstants[2] = json["blend_constants"][2];
        create_info.blendConstants[3] = json["blend_constants"][3];
        
        return create_info;
    }
    
    const vk::Bool32
    vk_pipeline_manager::get_bool_data( const std::string& data, const std::string& problem_location ) const noexcept
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
    const vk::PrimitiveTopology vk_pipeline_manager::get_topology_data( const std::string& data ) const noexcept
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
    const vk::PolygonMode vk_pipeline_manager::get_polygon_mode_data( const std::string& data ) const noexcept
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
    const vk::CullModeFlags vk_pipeline_manager::get_cull_mode_data( const std::string& data ) const noexcept
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
    const vk::FrontFace vk_pipeline_manager::get_front_face_data( const std::string& data ) const noexcept
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
    const vk::SampleCountFlagBits vk_pipeline_manager::get_rasterization_samples_data( const std::uint32_t data ) const noexcept
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
}
