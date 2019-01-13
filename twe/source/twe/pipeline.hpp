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

#ifndef ENGINE_PIPELINE_HPP
#define ENGINE_PIPELINE_HPP

#include <nlohmann/json.hpp>

#include "twe_core.hpp"
#include "shader_manager.hpp"
#include "graphics/vertex.hpp"
#include "utilities/file_io.hpp"
#include "utilities/log.hpp"

namespace twe
{
    enum class pipeline_type
    {
        graphics,
        compute
    };
    
    enum class dynamic_state_type
    {
        viewport = VK_DYNAMIC_STATE_VIEWPORT,
        scissor = VK_DYNAMIC_STATE_SCISSOR,
        line_width = VK_DYNAMIC_STATE_LINE_WIDTH,
        depth_bias = VK_DYNAMIC_STATE_DEPTH_BIAS,
        blend_constants = VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        depth_bounds = VK_DYNAMIC_STATE_DEPTH_BOUNDS,
        stencil_compare_mask = VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
        stencil_write_mask = VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
        stencil_reference = VK_DYNAMIC_STATE_STENCIL_REFERENCE,
    };
    
    struct pipeline_create_info
    {
        vk::Device* p_device_;
        vk::RenderPass* p_render_pass_;
        
        std::string pipeline_json_;
        
        std::vector<vk::Viewport> viewports_;
        std::vector<vk::Rect2D> scissors_;
        
        shader_manager* p_shader_manager_;
        uint32_t vert_shader_id_;
        uint32_t frag_shader_id_;
        
        pipeline_create_info& set_device( vk::Device* p_device )
        {
            p_device_ = p_device;
            
            return *this;
        }
        pipeline_create_info& set_render_pass( vk::RenderPass* p_render_pass )
        {
            p_render_pass_ = p_render_pass;
            
            return *this;
        }
        pipeline_create_info& set_pipeline_definition( const std::string& pipeline_json )
        {
            pipeline_json_ = pipeline_json;
            
            return *this;
        }
        pipeline_create_info& set_viewports( const std::vector<vk::Viewport>& viewports )
        {
            viewports_ = viewports;
            
            return *this;
        }
        pipeline_create_info& set_scissors( const std::vector<vk::Rect2D>& scissors )
        {
            scissors_ = scissors;
            
            return *this;
        }
        pipeline_create_info& set_shader_manager( shader_manager* p_shader_manager )
        {
            p_shader_manager_ = p_shader_manager;
            
            return *this;
        }
        pipeline_create_info& set_shader_ids( uint32_t vert_shader_id, uint32_t frag_shader_id )
        {
            vert_shader_id_ = vert_shader_id;
            frag_shader_id_ = frag_shader_id;
            
            return *this;
        }
    };
    
    template<pipeline_type T>
    class pipeline
    {
    public:
        pipeline( ) = default;
        pipeline( const pipeline_create_info& create_info )
            :
            vert_shader_id_( create_info.vert_shader_id_ ),
            frag_shader_id_( create_info.frag_shader_id_ )
        {
            const auto json = nlohmann::json::parse( read_from_file( create_info.pipeline_json_ ));
    
            auto get_bool_data = [&create_info]( const std::string& data, const std::string& error_location, const bool default_return_value = false )
            {
                if ( data == "true" )
                {
                    return static_cast<vk::Bool32>( true );
                }
                else if ( data == "false" )
                {
                    return static_cast<vk::Bool32>( false );
                }
                else
                {
                    core_warn( error_location + " parameter of pipeline definition \"{0}\" is incorrect. Check json file. default parameter \"false\" will be used instead.",
                               create_info.pipeline_json_ );
            
                    return static_cast<vk::Bool32>( default_return_value );
                }
            };
            auto get_float_data = [&create_info]( const float data, const std::string& error_location, const float default_return_value = .0f )
            {
                return data;
            };
            auto get_int_data = [&create_info]( const int data, const std::string& error_location, const int default_return_value = 0 )
            {
                return data;
            };
    
            auto get_topology = [&create_info]( const std::string& data )
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
                    core_warn( "rasterization/topology parameter of pipeline definition \"{0}\" is incorrect. Check json file. Default parameter \"triangle_list\" will be used instead.",
                               create_info.pipeline_json_ );
            
                    return vk::PrimitiveTopology::eTriangleList;
                }
            };
            auto get_polygon_mode = [&create_info]( const std::string& data )
            {
                if ( data == "point" )
                {
                    return vk::PolygonMode::ePoint;
                }
                else if ( data == "line" )
                {
                    return vk::PolygonMode::eLine;
                }
                else if ( data == "fill" )
                {
                    return vk::PolygonMode::eFill;
                }
                else
                {
                    core_warn( "rasterization/polygon_mode parameter of pipeline definition \"{0}\" is incorrect. Check json file. Default parameter \"fill\" will be used instead.",
                               create_info.pipeline_json_ );
            
                    return vk::PolygonMode::eFill;
                }
            };
            auto get_cull_mode = [&create_info]( const std::string& data )
            {
                if ( data == "none" )
                {
                    return vk::CullModeFlagBits::eNone;
                }
                else if ( data == "front" )
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
            
                    core_warn( "rasterization/cull_mode parameter of pipeline definition \"{0}\" is incorrect. Check json file. Default parameter \"none\" will be used instead.",
                               create_info.pipeline_json_ );
            
                    return vk::CullModeFlagBits::eNone;
                }
            };
            auto get_front_face = [&create_info]( const std::string& data )
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
                    core_warn( "rasterization/front_face parameter of pipeline definition \"{0}\" is incorrect. Check json file. Default parameter \"counter_clockwise\" will be used instead. ",
                               create_info.pipeline_json_ );
            
                    return vk::FrontFace::eCounterClockwise;
                }
            };
            auto get_rasterization_samples = [&create_info]( const int data )
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
                    core_warn(
                        "multisample/rasterization_samples parameter of pipeline definition \"{0}\" is incorrect. Check json file. Default parameter \"counter_clockwise\" will be used instead.",
                        create_info.pipeline_json_ );
            
                    return vk::SampleCountFlagBits::e1;
                }
            };
            auto get_logic_op = [&create_info]( const std::string& data )
            {
                if ( data == "copy" )
                {
                    return vk::LogicOp::eCopy;
                }
                else if ( data == "clear" )
                {
                    return vk::LogicOp::eClear;
                }
                else if ( data == "and" )
                {
                    return vk::LogicOp::eAnd;
                }
                else if ( data == "and_reverse" )
                {
                    return vk::LogicOp::eAndReverse;
                }
                else if ( data == "and_inverted" )
                {
                    return vk::LogicOp::eAndInverted;
                }
                else if ( data == "no_op" )
                {
                    return vk::LogicOp::eNoOp;
                }
                else if ( data == "xor" )
                {
                    return vk::LogicOp::eXor;
                }
                else if ( data == "or" )
                {
                    return vk::LogicOp::eOr;
                }
                else if ( data == "nor" )
                {
                    return vk::LogicOp::eNor;
                }
                else if ( data == "equivalent" )
                {
                    return vk::LogicOp::eEquivalent;
                }
                else if ( data == "invert" )
                {
                    return vk::LogicOp::eInvert;
                }
                else if ( data == "or_reverse" )
                {
                    return vk::LogicOp::eOrReverse;
                }
                else if ( data == "copy_inverted" )
                {
                    return vk::LogicOp::eCopyInverted;
                }
                else if ( data == "or_inverted" )
                {
                    return vk::LogicOp::eOrInverted;
                }
                else if ( data == "nand" )
                {
                    return vk::LogicOp::eNand;
                }
                else if ( data == "set" )
                {
                    return vk::LogicOp::eSet;
                }
                else
                {
                    core_warn( "colour_blend/logic_op parameter of pipeline definition \"{0}\" is incorrect. Check json file. Default parameter \"copy\" will be used instead.",
                               create_info.pipeline_json_ );
            
                    return vk::LogicOp::eCopy;
                }
            };
            
            auto get_blend_factor = [&create_info]( const std::string& data )
            {
                if ( data == "zero" )
                {
                    return vk::BlendFactor::eZero;
                }
                else if ( data == "one" )
                {
                    return vk::BlendFactor::eOne;
                }
                else if ( data == "src_colour" )
                {
                    return vk::BlendFactor::eSrcColor;
                }
                else if ( data == "one_minus_src_colour" )
                {
                    return vk::BlendFactor::eOneMinusSrcColor;
                }
                else if ( data == "dst_colour" )
                {
                    return vk::BlendFactor::eDstColor;
                }
                else if ( data == "one_minus_dst_colour" )
                {
                    return vk::BlendFactor::eOneMinusDstColor;
                }
                else if ( data == "src_alpha" )
                {
                    return vk::BlendFactor::eSrcAlpha;
                }
                else if ( data == "one_minus_src_alpha" )
                {
                    return vk::BlendFactor::eOneMinusSrcAlpha;
                }
                else if ( data == "dst_alpha" )
                {
                    return vk::BlendFactor::eDstAlpha;
                }
                else if ( data == "one_minus_dst_alpha" )
                {
                    return vk::BlendFactor::eOneMinusDstAlpha;
                }
                else if ( data == "constant_colour" )
                {
                    return vk::BlendFactor::eConstantColor;
                }
                else if ( data == "one_minus_constant_colour" )
                {
                    return vk::BlendFactor::eOneMinusConstantColor;
                }
                else if ( data == "constant_alpha" )
                {
                    return vk::BlendFactor::eConstantAlpha;
                }
                else if ( data == "one_minus_constant_alpha" )
                {
                    return vk::BlendFactor::eOneMinusConstantAlpha;
                }
                else if ( data == "src_alpha_saturate" )
                {
                    return vk::BlendFactor::eSrcAlphaSaturate;
                }
                else if ( data == "src_1_colour" )
                {
                    return vk::BlendFactor::eSrc1Color;
                }
                else if ( data == "one_minus_src_1_colour" )
                {
                    return vk::BlendFactor::eOneMinusSrc1Color;
                }
                else if ( data == "src_1_alpha" )
                {
                    return vk::BlendFactor::eSrc1Alpha;
                }
                else if ( data == "one_minus_src_1_alpha" )
                {
                    return vk::BlendFactor::eOneMinusSrc1Alpha;
                }
                else
                {
                    core_warn( "colour_blend_attachment/blend_factor parameter of pipeline definition \"{0}\" is incorrect. Check json file. Default parameter \"zero\" will be used instead.",
                        create_info.pipeline_json_ );
    
                    return vk::BlendFactor::eZero;
                }
            };
            
            auto get_blend_op = [&create_info]( const std::string& data )
            {
                if( data == "add" )
                {
                    return vk::BlendOp::eAdd;
                }
                else if ( data == "substract" )
                {
                    return vk::BlendOp::eSubtract;
                }
                else if ( data == "reverse_substract" )
                {
                    return vk::BlendOp::eReverseSubtract;
                }
                else if ( data == "min" )
                {
                    return vk::BlendOp::eMin;
                }
                else if ( data == "max" )
                {
                    return vk::BlendOp::eMax;
                }
                else
                {
                    core_warn( "colour_blend_attachment/blend_op parameter of pipeline definition \"{0}\" is incorrect. Check json file. Default parameter \"zero\" will be used instead.",
                        create_info.pipeline_json_);
                    
                    return vk::BlendOp::eAdd;
                }
            };
            
            auto get_colour_write_mask = [&create_info]( const nlohmann::json& json )
            {
                vk::ColorComponentFlags flags;
                
                for( const auto& data : json )
                {
                    if ( data == "R" || data == "r" )
                    {
                        flags |= vk::ColorComponentFlagBits::eR;
                    }
                    else if ( data == "G" || data == "g" )
                    {
                        flags |= vk::ColorComponentFlagBits::eG;
                    }
                    else if ( data == "B" || data == "b" )
                    {
                        flags |= vk::ColorComponentFlagBits::eB;
                    }
                    else if ( data == "A" || data == "a" )
                    {
                        flags |= vk::ColorComponentFlagBits::eA;
                    }
                }
                
                return flags;
            };
            
            auto get_colour_blend_attachments = [=]( const nlohmann::json& json )
            {
                std::vector<vk::PipelineColorBlendAttachmentState> states;
                
                for( const auto& data : json )
                {
                    const auto colour_blend_attachment = vk::PipelineColorBlendAttachmentState( )
                        .setBlendEnable( get_bool_data( data["blend_enable"], "colour_blend_attachment/blend_enable" ) )
                        .setSrcColorBlendFactor( get_blend_factor( data["src_colour_blend_factor"] ) )
                        .setDstColorBlendFactor( get_blend_factor( data["dst_colour_blend_factor"] ) )
                        .setColorBlendOp( get_blend_op( data["colour_blend_op"] ) )
                        .setSrcAlphaBlendFactor( get_blend_factor( data["src_alpha_blend_factor"] ) )
                        .setDstAlphaBlendFactor( get_blend_factor( data["dst_alpha_blend_factor"] ) )
                        .setAlphaBlendOp( get_blend_op( data["alpha_blend_op"] ) )
                        .setColorWriteMask( get_colour_write_mask( data["colour_write_mask"] ) );
                    
                    states.push_back( colour_blend_attachment );
                }
                
                return states;
            };
            
            auto get_dynamic_states = [&create_info]( const std::vector<std::string>& names )
            {
                std::vector<vk::DynamicState> states;
                
                for( const auto data : names )
                {
                    if( data == "viewport" )
                    {
                        states.emplace_back( vk::DynamicState::eViewport );
                    }
                    else if ( data == "scissors" )
                    {
                        states.emplace_back( vk::DynamicState::eScissor );
                    }
                    else if ( data == "line_width" )
                    {
                        states.emplace_back( vk::DynamicState::eLineWidth );
                    }
                    else if ( data == "depth_bias" )
                    {
                        states.emplace_back( vk::DynamicState::eDepthBias );
                    }
                    else if ( data == "blend_constants" )
                    {
                        states.emplace_back( vk::DynamicState::eBlendConstants );
                    }
                    else if ( data == "stencil_compare_mask" )
                    {
                        states.emplace_back( vk::DynamicState::eStencilCompareMask );
                    }
                    else if ( data == "stencil_write_mask" )
                    {
                        states.emplace_back( vk::DynamicState::eStencilWriteMask );
                    }
                    else if ( data == "stencil_reference" )
                    {
                        states.emplace_back( vk::DynamicState::eStencilReference );
                    }
                    
                    // TODO: add support for extensions.
                }
                
                return states;
            };
            
            const vk::PipelineShaderStageCreateInfo shader_stage_create_infos[] = {
                create_info.p_shader_manager_->find<shader_type::vertex>( vert_shader_id_ ).get_shader_stage_create_info( ),
                create_info.p_shader_manager_->find<shader_type::fragment>( frag_shader_id_ ).get_shader_stage_create_info( )
            };
    
            //
            vertex_input_config_.bindings_.emplace_back(
                vk::VertexInputBindingDescription( )
                    .setBinding( 0 )
                    .setStride( sizeof( vertex ) )
                    .setInputRate( vk::VertexInputRate::eVertex ) );
            
            vertex_input_config_.attributes_.emplace_back(
                vk::VertexInputAttributeDescription( )
                    .setBinding( 0 )
                    .setLocation( 0 )
                    .setFormat( vk::Format::eR32G32B32Sfloat )
                    .setOffset( static_cast<uint32_t>( offsetof( struct vertex, position_ ) ) )
                );
            
            vertex_input_config_.attributes_.emplace_back(
                vk::VertexInputAttributeDescription( )
                    .setBinding( 0 )
                    .setLocation( 1 )
                    .setFormat( vk::Format::eR32G32B32A32Sfloat )
                    .setOffset( static_cast<uint32_t>( offsetof( struct vertex, colour_ ) ) )
                );
            //
            
            
            const auto colour_blend_attachment_states = get_colour_blend_attachments( json["colour_blend_attachments"] );
            
            dynamic_states_ = get_dynamic_states( json["dynamic_states"] );
            
            const auto vertex_input_state = vk::PipelineVertexInputStateCreateInfo( )
                .setVertexBindingDescriptionCount( static_cast<uint32_t>( vertex_input_config_.bindings_.size() ) )
                .setPVertexBindingDescriptions( vertex_input_config_.bindings_.data() )
                .setVertexAttributeDescriptionCount( static_cast<uint32_t>( vertex_input_config_.attributes_.size() ) )
                .setPVertexAttributeDescriptions( vertex_input_config_.attributes_.data() );
            
            const auto input_assembly_state = vk::PipelineInputAssemblyStateCreateInfo( )
                .setPrimitiveRestartEnable( get_bool_data( json["input_assembly"]["primitive_restart_enable"], "input_assembly/primitive_restart_enable" ) )
                .setTopology( get_topology( json["input_assembly"]["topology"] ) );
            
            const auto viewport_state = vk::PipelineViewportStateCreateInfo( )
                .setViewportCount( static_cast<uint32_t>( create_info.viewports_.size() ) )
                .setPViewports( create_info.viewports_.data() )
                .setScissorCount( static_cast<uint32_t>( create_info.scissors_.size() ) )
                .setPScissors( create_info.scissors_.data() );
            
            const auto rasterization_state = vk::PipelineRasterizationStateCreateInfo( )
                .setDepthClampEnable( get_bool_data( json["rasterization"]["depth_clamp_enable"], "rasterization/depth_clamp_enable" ) )
                .setRasterizerDiscardEnable( get_bool_data( json["rasterization"]["rasterization_discard_enable"], "rasterization/rasterization_discard_enable" ) )
                .setPolygonMode( get_polygon_mode( json["rasterization"]["polygon_mode"] ) )
                .setCullMode( get_cull_mode( json["rasterization"]["cull_mode"] ) )
                .setFrontFace( get_front_face( json["rasterization"]["front_face"] ) )
                .setDepthBiasEnable( get_bool_data( json["rasterization"]["depth_bias_enable"], "rasterization/depth_bias_enable" ) )
                .setDepthBiasConstantFactor( get_float_data( json["rasterization"]["depth_bias_constant_factor"], "rasterization/depth_bias_constant_factor" ) )
                .setDepthBiasClamp( get_float_data( json["rasterization"]["depth_bias_clamp"], "rasterization/depth_bias_clamp" ) )
                .setDepthBiasSlopeFactor( get_float_data( json["rasterization"]["depth_bias_slope_factor"], "rasterization/depth_bias_slope_factor" ) )
                .setLineWidth( get_int_data( json["rasterization"]["line_width"], "rasterization/line_width", 1 ) );
            
            const auto multisampling_state = vk::PipelineMultisampleStateCreateInfo( )
                .setRasterizationSamples( get_rasterization_samples( json["multisampling"]["rasterization_samples"] ) )
                .setSampleShadingEnable( get_bool_data( json["multisampling"]["sample_shading_enable"], "multisampling/sample_shading_enable" ) )
                .setMinSampleShading( get_float_data( json["multisampling"]["min_sample_shading"], "multisampling/min_sample_shading", 1.0f ) )
                .setPSampleMask( nullptr )
                .setAlphaToCoverageEnable( get_bool_data( json["multisampling"]["alpha_to_coverage_enable"], "multisampling/alpha_to_coverage_enable" ) )
                .setAlphaToOneEnable( get_bool_data( json["multisampling"]["alpha_to_one_enable"], "multisampling/alpha_to_one_enable" ) );
            
            const auto colour_blend_state = vk::PipelineColorBlendStateCreateInfo( )
                .setLogicOpEnable( get_bool_data( json["colour_blend"]["logic_op_enable"], "colour_blend/logic_op_enable" ) )
                .setLogicOp( get_logic_op( json["colour_blend"]["logic_op"] ) )
                .setAttachmentCount( static_cast<uint32_t>( colour_blend_attachment_states.size() ) )
                .setPAttachments( colour_blend_attachment_states.data() )
                .setBlendConstants( std::array<float, 4>{
                    get_float_data( json["colour_blend"]["blend_constants"][0], "colour_blend/blend_constants" ),
                    get_float_data( json["colour_blend"]["blend_constants"][1], "colour_blend/blend_constants" ),
                    get_float_data( json["colour_blend"]["blend_constants"][2], "colour_blend/blend_constants" ),
                    get_float_data( json["colour_blend"]["blend_constants"][3], "colour_blend/blend_constants" ) } );
            
            const auto dynamic_state_create_info = vk::PipelineDynamicStateCreateInfo( )
                .setDynamicStateCount( static_cast<uint32_t>( dynamic_states_.size() ) )
                .setPDynamicStates( dynamic_states_.data() );
            
            const auto layout_create_info = vk::PipelineLayoutCreateInfo( );
            
            layout_ = create_info.p_device_->createPipelineLayoutUnique( layout_create_info );
            
            if( T == pipeline_type::graphics )
            {
                const auto graphics_create_info = vk::GraphicsPipelineCreateInfo( )
                    .setStageCount( sizeof( shader_stage_create_infos ) / sizeof( shader_stage_create_infos[0] ) )
                    .setPStages( shader_stage_create_infos )
                    .setPVertexInputState( &vertex_input_state )
                    .setPInputAssemblyState( &input_assembly_state )
                    .setPTessellationState( nullptr )
                    .setPViewportState( &viewport_state )
                    .setPRasterizationState( &rasterization_state )
                    .setPMultisampleState( &multisampling_state )
                    .setPDepthStencilState( nullptr )
                    .setPColorBlendState( &colour_blend_state )
                    .setPDynamicState( &dynamic_state_create_info )
                    .setLayout( layout_.get() )
                    .setRenderPass( *create_info.p_render_pass_ )
                    .setSubpass( 0 )
                    .setBasePipelineHandle( nullptr )
                    .setBasePipelineIndex( -1 );
                
                auto test = create_info.p_device_->createGraphicsPipelinesUnique( nullptr, graphics_create_info );
                
                pipeline_ = std::move( test[0] );
            }
            else if ( T == pipeline_type::compute )
            {
                const auto create_info = vk::ComputePipelineCreateInfo( );
            }
        }
        pipeline( vk::UniquePipeline&& pipeline, uint32_t vert_shader_id, uint32_t frag_shader_id, uint32_t pipeline_layout_id, uint32_t pipeline_cache_id )
            :
            pipeline_( std::move( pipeline ) ),
            vert_shader_id_( vert_shader_id ),
            frag_shader_id_( frag_shader_id ),
            pipeline_layout_id_( pipeline_layout_id ),
            pipeline_cache_id_( pipeline_cache_id )
        { }
        pipeline( const pipeline& rhs ) noexcept = delete;
        pipeline( pipeline&& rhs ) noexcept
        {
            *this = std::move( rhs );
        }
        ~pipeline( ) = default;
        
        pipeline& operator=( const pipeline& rhs ) noexcept = delete;
        pipeline& operator=( pipeline&& rhs ) noexcept
        {
            if( this != &rhs )
            {
                pipeline_ = std::move( rhs.pipeline_ );
                
                dynamic_states_ = std::move( rhs.dynamic_states_ );
                
                vertex_input_config_ = std::move( rhs.vertex_input_config_ );
            
                vert_shader_id_ = rhs.vert_shader_id_;
                rhs.vert_shader_id_ = 0;
                
                frag_shader_id_ = rhs.frag_shader_id_;
                rhs.frag_shader_id_ = 0;
                
                pipeline_layout_id_ = rhs.pipeline_layout_id_;
                rhs.pipeline_layout_id_ = 0;
                
                pipeline_cache_id_ = rhs.pipeline_cache_id_;
                rhs.pipeline_cache_id_ = 0;
            }
            
            return *this;
        }
        
        template<dynamic_state_type type, class = std::enable_if_t<type == dynamic_state_type::viewport>>
        void set_dynamic_state( const vk::CommandBuffer& buffer, uint32_t first_viewport, std::vector<vk::Viewport>& viewports ) const
        {
            if ( std::find( dynamic_states_.cbegin( ), dynamic_states_.cend( ), vk::DynamicState::eViewport ) != dynamic_states_.cend() )
            {
                buffer.setViewport( first_viewport, viewports );
            }
            else
            {
                core_warn( "Attempted to set dynamic state: viewport, while it is not enabled in the graphics pipeline." );
            }
        }
        template<dynamic_state_type type, class = std::enable_if_t<type == dynamic_state_type::scissor>>
        void set_dynamic_state( const vk::CommandBuffer& buffer, uint32_t first_scissor, std::vector<vk::Rect2D>& scissors ) const
        {
            if ( std::find( dynamic_states_.cbegin(), dynamic_states_.cend(), vk::DynamicState::eScissor ) != dynamic_states_.cend() )
            {
                buffer.setScissor( first_scissor, scissors );
            }
            else
            {
                core_warn( "Attempted to set dynamic state: scissor, while it is not enabled in the graphics pipeline." );
            }
        }
        template<dynamic_state_type type, class = std::enable_if_t<type == dynamic_state_type::line_width>>
        void set_dynamic_state( const vk::CommandBuffer& buffer, float line_width ) const
        {
            if ( std::find( dynamic_states_.cbegin(), dynamic_states_.cend( ), vk::DynamicState::eLineWidth ) != dynamic_states_.cend() )
            {
                buffer.setLineWidth( line_width );
            }
            else
            {
                core_warn( "Attempted to set dynamic state: line_width, while it is not enabled in the graphics pipeline." );
            }
        }
        template<dynamic_state_type type, class = std::enable_if_t<type == dynamic_state_type::depth_bias>>
        void set_dynamic_state( const vk::CommandBuffer& buffer, float depth_bias_constant_factor, float depth_bias_clamp, float depth_bias_slope_factor ) const
        {
            if ( std::find( dynamic_states_.cbegin(), dynamic_states_.cend( ), vk::DynamicState::eDepthBias ) != dynamic_states_.cend() )
            {
                buffer.setDepthBias( depth_bias_constant_factor, depth_bias_clamp, depth_bias_slope_factor );
            }
            else
            {
                core_warn( "Attempted to set dynamic state: depth_bias, while it is not enabled in the graphics pipeline." );
            }
        }
        template<dynamic_state_type type, class = std::enable_if_t<type == dynamic_state_type::blend_constants>>
        void set_dynamic_state( const vk::CommandBuffer& buffer, const float* blend_constants ) const
        {
            if ( std::find( dynamic_states_.cbegin(), dynamic_states_.cend(), vk::DynamicState::eBlendConstants ) != dynamic_states_.cend() )
            {
                buffer.setBlendConstants( blend_constants );
            }
            else
            {
                core_warn( "Attempted to set dynamic state: blend_constants, while it is not enabled in the graphics pipeline." );
            }
        }
        template<dynamic_state_type type, class = std::enable_if_t<type == dynamic_state_type::depth_bounds>>
        void set_dynamic_state( const vk::CommandBuffer& buffer, float min_depth_bound, float max_depth_bound ) const
        {
            if ( std::find( dynamic_states_.cbegin(), dynamic_states_.cend( ), vk::DynamicState::eDepthBounds ) != dynamic_states_.cend() )
            {
                buffer.setDepthBounds( min_depth_bound, max_depth_bound );
            }
            else
            {
                core_warn( "Attempted to set dynamic state: depth_bounds, while it is not enabled in the graphics pipeline." );
            }
        }
        template<dynamic_state_type type>
        std::enable_if_t<type == dynamic_state_type::stencil_compare_mask, void>
        set_dynamic_state( const vk::CommandBuffer& buffer, vk::StencilFaceFlags flags, uint32_t compare_mask ) const
        {
            if ( std::find( dynamic_states_.cbegin(), dynamic_states_.cend( ), vk::DynamicState::eStencilCompareMask ) != dynamic_states_.cend( ) )
            {
                buffer.setStencilCompareMask( flags, compare_mask );
            }
            else
            {
                core_warn( "Attempted to set dynamic state: stencil_compare_mask, while it is not enabled in the graphics pipeline." );
            }
        }
        template<dynamic_state_type type>
        std::enable_if_t<type == dynamic_state_type::stencil_write_mask, void>
        set_dynamic_state( const vk::CommandBuffer& buffer, vk::StencilFaceFlags flags, uint32_t write_mask ) const
        {
            if ( std::find( dynamic_states_.cbegin(), dynamic_states_.cend( ), vk::DynamicState::eStencilWriteMask ) != dynamic_states_.cend( ) )
            {
                buffer.setStencilWriteMask( flags, write_mask );
            }
            else
            {
                core_warn( "Attempted to set dynamic state: stencil_write_mask, while it is not enabled in the graphics pipeline." );
            }
        }
        template<dynamic_state_type type>
        std::enable_if_t<type == dynamic_state_type::stencil_reference, void>
        set_dynamic_state( const vk::CommandBuffer& buffer, vk::StencilFaceFlags flags, uint32_t reference ) const
        {
            if ( std::find( dynamic_states_.cbegin(), dynamic_states_.cend( ), vk::DynamicState::eStencilReference ) != dynamic_states_.cend( ) )
            {
                buffer.setStencilReference( flags, reference );
            }
            else
            {
                core_warn( "Attempted to set dynamic state: stencil_reference, while it is not enabled in the graphics pipeline." );
            }
        }
        
        void bind( const vk::CommandBuffer& buffer ) const
        {
            if( T == pipeline_type::graphics )
            {
                buffer.bindPipeline( vk::PipelineBindPoint::eGraphics, pipeline_.get() );
            }
            else if ( T == pipeline_type::compute )
            {
                buffer.bindPipeline( vk::PipelineBindPoint::eCompute, pipeline_.get() );
            }
        }
        
    private:
        vk::UniquePipeline pipeline_;
        
        std::vector<vk::DynamicState> dynamic_states_;
        
        //TEMPORARY:
        vk::UniquePipelineLayout layout_;
        //
        
        vertex_input_config vertex_input_config_;
        
        uint32_t vert_shader_id_;
        uint32_t frag_shader_id_;
        uint32_t pipeline_layout_id_;
        uint32_t pipeline_cache_id_;
    };
    
    using graphics_pipeline = pipeline<pipeline_type::graphics>;
    using compute_pipeline = pipeline<pipeline_type::compute>;
}

#endif //ENGINE_PIPELINE_HPP
