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

#include "pipeline.hpp"

namespace lcl::vulkan
{
    template<>
    const json_return_t<graphics_pipeline::values::bool32>
    graphics_pipeline::parse_json_value<graphics_pipeline::values::bool32>( 
        const nlohmann::json& json,  const std::string_view value_name, const std::string_view pipeline_name ) const
    {
        const auto value = json[value_name.data( )];
        assert( value.is_string( ) );

        const auto it = bool_values_.find( value );
        assert( it != bool_values_.cend( ) );

        return it->second;
    }

    template<>
    const json_return_t<graphics_pipeline::values::uint32>
    graphics_pipeline::parse_json_value<graphics_pipeline::values::uint32>( 
        const nlohmann::json& json,  const std::string_view value_name, const std::string_view pipeline_name ) const
    {
        const auto value = json[value_name.data( )];
        assert( value.is_number_unsigned( ) );

        return value;
    }

    template<>
    const json_return_t<graphics_pipeline::values::floating_point>
    graphics_pipeline::parse_json_value<graphics_pipeline::values::floating_point>( 
        const nlohmann::json& json,  const std::string_view value_name, const std::string_view pipeline_name ) const
    {
        const auto value = json[value_name.data( )];
        assert( value.is_number_float( ) );

        return value;
    }

    template<>
    const json_return_t<graphics_pipeline::values::topology>
    graphics_pipeline::parse_json_value<graphics_pipeline::values::topology>( 
        const nlohmann::json& json, const std::string_view value_name, const std::string_view pipeline_name ) const
    {
        const auto value = json[value_name.data( )];
        assert( value.is_string( ) );

        const auto it = supported_topologies_.find( value );
        assert( it != supported_topologies_.cend( ) );

        return it->second;
    }

    template<>
    const json_return_t<graphics_pipeline::values::cull_mode>
    graphics_pipeline::parse_json_value<graphics_pipeline::values::cull_mode>( 
        const nlohmann::json& json, const std::string_view value_name, const std::string_view pipeline_name ) const
    {
        const auto value = json[value_name.data( )];
        assert( value.is_string( ) );
        
        const auto it = supported_cull_modes_.find( value );
        assert( it != supported_cull_modes_.cend( ) );

        return it->second;
    }

    template<>
    const json_return_t<graphics_pipeline::values::polygon_mode>
    graphics_pipeline::parse_json_value<graphics_pipeline::values::polygon_mode>( 
        const nlohmann::json& json, const std::string_view value_name, const std::string_view pipeline_name ) const
    {
        const auto value = json[value_name.data( )];
        assert( value.is_string( ) );

        const auto it = supported_polygon_modes_.find( value );
        assert( it != supported_polygon_modes_.cend( ) );

        return it->second;
    }


    template<>
    const json_return_t<graphics_pipeline::values::front_face>
    graphics_pipeline::parse_json_value<graphics_pipeline::values::front_face>( 
        const nlohmann::json& json, const std::string_view value_name, const std::string_view pipeline_name ) const
    {
        const auto value = json[value_name.data( )];
        assert( value.is_string( ) );

        const auto it = supported_front_faces_.find( value );
        assert( it != supported_front_faces_.cend( ) );

        return it->second;
    }

    template<>
    const json_return_t<graphics_pipeline::values::sample_count>
    graphics_pipeline::parse_json_value<graphics_pipeline::values::sample_count>( 
        const nlohmann::json& json, const std::string_view value_name, const std::string_view pipeline_name ) const
    {
        const auto value = json[value_name.data( )];
        assert( value.is_number_unsigned( ) );

        const auto it = supported_sample_counts_.find( value );
        assert( it != supported_sample_counts_.cend( ) );

        return it->second;
    }

    template<>
    const json_return_t<graphics_pipeline::values::blend_factor>
    graphics_pipeline::parse_json_value<graphics_pipeline::values::blend_factor>( 
        const nlohmann::json& json, const std::string_view value_name, const std::string_view pipeline_name ) const
    {
        const auto value = json[value_name.data( )];
        assert( value.is_string( ) );

        const auto it = supported_blend_factors_.find( value );
        assert( it != supported_blend_factors_.cend( ) );

        return it->second;
    }

    template<>
    const json_return_t<graphics_pipeline::values::blend_op>
    graphics_pipeline::parse_json_value<graphics_pipeline::values::blend_op>( 
        const nlohmann::json& json, const std::string_view value_name, const std::string_view pipeline_name ) const
    {
        const auto value = json[value_name.data( )];
        assert( value.is_string( ) );

        const auto it = supported_blend_ops_.find( value );
        assert( it != supported_blend_ops_.cend( ) );

        return it->second;
    }

    template<>
    const json_return_t<graphics_pipeline::values::logic_op>
    graphics_pipeline::parse_json_value<graphics_pipeline::values::logic_op>( 
        const nlohmann::json& json, const std::string_view value_name, const std::string_view pipeline_name ) const
    {
        const auto value = json[value_name.data( )];
        assert( value.is_string( ) );

        const auto it = supported_logic_ops_.find( value );
        assert( it != supported_logic_ops_.cend( ) );

        return it->second;
    }
    
    template<>
    const json_return_t<graphics_pipeline::values::colour_components> 
    graphics_pipeline::parse_json_value<graphics_pipeline::values::colour_components>(
        const nlohmann::json& json, const std::string_view value_name, const std::string_view pipeline_name ) const
    {
        const auto values = json[value_name.data( )];
        assert( values.is_array( ) );

        vk::ColorComponentFlags flags;

        for( auto& value : values )
        {
            assert( value.is_string( ) );

            const auto it = supported_colour_components_.find( value );
            assert( it != supported_colour_components_.cend( ) );

            flags = flags | it->second;
        }

        return flags;
    }

    template<>
    const json_return_t<graphics_pipeline::values::blend_constants>
    graphics_pipeline::parse_json_value<graphics_pipeline::values::blend_constants>(
        const nlohmann::json& json, const std::string_view value_name, const std::string_view pipeline_name ) const
    {
        const auto values = json[value_name.data( )];
        assert( values.is_array( ) );

        for( auto value : values )
        {
            assert( value.is_number_float( ) );
        }

        return values;
    }

    /*!
     *
     */
    template<>
    const json_return_t<graphics_pipeline::sections::input_assembly>
    graphics_pipeline::parse_json_section<graphics_pipeline::sections::input_assembly>( const nlohmann::json& json, const std::string_view pipeline_name ) const
    {
        const auto section = json["input_assembly"];

        using values = graphics_pipeline::values;

        const auto create_info = vk::PipelineInputAssemblyStateCreateInfo{ }
            .setPrimitiveRestartEnable( parse_json_value<values::bool32>( section, "primitive_restart_enable", pipeline_name ) )
            .setTopology( parse_json_value<values::topology>( section, "topology", pipeline_name ) );

        return create_info;        
    }

    template<>
    const json_return_t<graphics_pipeline::sections::rasterization>
    graphics_pipeline::parse_json_section<graphics_pipeline::sections::rasterization>( const nlohmann::json& json, const std::string_view pipeline_name ) const
    {
        const auto section = json["rasterization"];

        using values = graphics_pipeline::values;

        const auto create_info = vk::PipelineRasterizationStateCreateInfo{ }
            .setDepthClampEnable( parse_json_value<values::bool32>( section, "depth_clamp_enable", pipeline_name ) )
            .setRasterizerDiscardEnable( parse_json_value<values::bool32>( section, "rasterizer_discard_enable", pipeline_name ) )
            .setPolygonMode( parse_json_value<values::polygon_mode>( section, "polygon_mode", pipeline_name ) )
            .setCullMode( parse_json_value<values::cull_mode>( section, "cull_mode", pipeline_name ) )
            .setFrontFace( parse_json_value<values::front_face>( section, "front_face", pipeline_name ) )
            .setDepthBiasEnable( parse_json_value<values::bool32>( section, "depth_bias_enable", pipeline_name ) )
            .setDepthBiasConstantFactor( parse_json_value<values::floating_point>( section, "depth_bias_constant_factor", pipeline_name ) )
            .setDepthBiasClamp( parse_json_value<values::floating_point>( section, "depth_bias_clamp", pipeline_name ) )
            .setDepthBiasSlopeFactor( parse_json_value<values::floating_point>( section, "depth_bias_slope_factor", pipeline_name ) )
            .setLineWidth( parse_json_value<values::floating_point>( section, "line_width", pipeline_name ) );

        return create_info;
    }

    template<>
    const json_return_t<graphics_pipeline::sections::multisampling>
    graphics_pipeline::parse_json_section<graphics_pipeline::sections::multisampling>( const nlohmann::json& json, const std::string_view pipeline_name ) const
    {
        const auto section = json["multisampling"];

        using values = graphics_pipeline::values;

        const auto create_info = vk::PipelineMultisampleStateCreateInfo{ }
            .setRasterizationSamples( parse_json_value<values::sample_count>( section, "rasterization_samples", pipeline_name ) )
            .setSampleShadingEnable( parse_json_value<values::bool32>( section, "sample_shading_enable", pipeline_name ) )
            .setMinSampleShading( parse_json_value<values::floating_point>( section, "min_sample_shading", pipeline_name ) )
            .setPSampleMask( nullptr )
            .setAlphaToCoverageEnable( parse_json_value<values::bool32>( section, "alpha_to_coverage_enable", pipeline_name ) )
            .setAlphaToOneEnable( parse_json_value<values::bool32>( section, "alpha_to_one_enable", pipeline_name ) );

        return create_info;
    }

    template<>
    const json_return_t<graphics_pipeline::sections::colour_blend_attachments>
    graphics_pipeline::parse_json_section<graphics_pipeline::sections::colour_blend_attachments>( const nlohmann::json& json,const std::string_view pipeline_name ) const
    {
        const auto section = json["colour_blend_attachments"];
        assert( section.is_array( ) );

        std::vector<vk::PipelineColorBlendAttachmentState> attachments;
        attachments.reserve( section.size( ) );

        using values = graphics_pipeline::values;

        for( auto& elem : section )
        {
            const auto state = vk::PipelineColorBlendAttachmentState{ }
                .setBlendEnable( parse_json_value<values::bool32>( elem, "blend_enable", pipeline_name ) )
                .setSrcColorBlendFactor( parse_json_value<values::blend_factor>( elem, "src_colour_blend_factor", pipeline_name ) )
                .setDstColorBlendFactor( parse_json_value<values::blend_factor>( elem, "dst_colour_blend_factor", pipeline_name ) )
                .setColorBlendOp( parse_json_value<values::blend_op>( elem, "colour_blend_op", pipeline_name ) )
                .setSrcAlphaBlendFactor( parse_json_value<values::blend_factor>( elem, "src_alpha_blend_factor", pipeline_name ) )
                .setDstAlphaBlendFactor( parse_json_value<values::blend_factor>( elem, "dst_alpha_blend_factor", pipeline_name ) )
                .setAlphaBlendOp( parse_json_value<values::blend_op>( elem, "alpha_blend_op", pipeline_name ) )
                .setColorWriteMask( parse_json_value<values::colour_components>( elem, "colour_write_mask", pipeline_name ) );

            attachments.push_back( state );
        }

        return attachments;
    }

    template<>
    const json_return_t<graphics_pipeline::sections::colour_blend>
    graphics_pipeline::parse_json_section<graphics_pipeline::sections::colour_blend>( const nlohmann::json& json, const std::string_view pipeline_name ) const
    {
        const auto section = json["colour_blend"];

        using value = graphics_pipeline::values;

        const auto colour_blend = vk::PipelineColorBlendStateCreateInfo{ }
            .setLogicOpEnable( parse_json_value<values::bool32>( section, "logic_op_enable", pipeline_name ) )
            .setLogicOp( parse_json_value<values::logic_op>( section, "logic_op", pipeline_name ) )
            .setBlendConstants( parse_json_value<values::blend_constants>( section, "blend_constants", pipeline_name ) );
        
        return colour_blend;
    }

    template<>
    const json_return_t<graphics_pipeline::sections::dynamic_states>
    graphics_pipeline::parse_json_section<graphics_pipeline::sections::dynamic_states>( const nlohmann::json& json, const std::string_view pipeline_name ) const
    {
        const auto values = json["dynamic_states"];
        assert( values.is_array( ) );

        std::vector<vk::DynamicState> states;
        states.reserve( values.size( ) );

        for ( auto& value : values )
        {
            assert( value.is_string( ) );

            const auto it = supported_dynamic_states_.find( value );
            assert( it != supported_dynamic_states_.cend( ) );

            states.push_back( it->second );
        }
        
        return states;
    }

    graphics_pipeline::pipeline( const graphics_pipeline::create_info& create_info )
        :
        vert_shader_id_( create_info.vert_shader_id_ ),
        frag_shader_id_( create_info.frag_shader_id_ )
    {
        const auto json = nlohmann::json::parse( bzr::read_from_file( create_info.pipeline_json_ ) );

        const vk::PipelineShaderStageCreateInfo shader_stage_create_infos[] = {
            create_info.p_shader_manager_->find<shader_type::vertex>( vert_shader_id_ ).get_shader_stage_create_info( ),
            create_info.p_shader_manager_->find<shader_type::fragment>( frag_shader_id_ ).get_shader_stage_create_info( )
        };

        const auto viewport_state = vk::PipelineViewportStateCreateInfo{ }
            .setViewportCount( static_cast<uint32_t>( create_info.viewports_.size() ) )
            .setPViewports( create_info.viewports_.data() )
            .setScissorCount( static_cast<uint32_t>( create_info.scissors_.size() ) )
            .setPScissors( create_info.scissors_.data() );

        vk::VertexInputBindingDescription binding_description[2];
        vk::VertexInputAttributeDescription attribute_description[2];

        binding_description[0] = vk::VertexInputBindingDescription{ }
            .setBinding( 0 )
            .setStride( sizeof( glm::vec3 ) )
            .setInputRate( vk::VertexInputRate::eVertex );

        binding_description[1] = vk::VertexInputBindingDescription{ }
            .setBinding( 1 )
            .setStride( sizeof( glm::vec4 ) )
            .setInputRate( vk::VertexInputRate::eVertex );

        attribute_description[0] = vk::VertexInputAttributeDescription{ }
            .setBinding( 0 )
            .setLocation( 0 )
            .setFormat( vk::Format::eR32G32B32Sfloat )
            .setOffset( 0 );

        attribute_description[1] = vk::VertexInputAttributeDescription{ }
            .setBinding( 1 )
            .setLocation( 1 )
            .setFormat( vk::Format::eR32G32B32A32Sfloat )
            .setOffset( 0 );

        const auto vertex_input_state = vk::PipelineVertexInputStateCreateInfo{ }
            .setVertexBindingDescriptionCount( sizeof( binding_description ) / sizeof( binding_description[0] ))
            .setPVertexBindingDescriptions( binding_description )
            .setVertexAttributeDescriptionCount( sizeof( attribute_description ) / sizeof( attribute_description[0] ) )
            .setPVertexAttributeDescriptions( attribute_description );

        const auto dynamic_states_data = parse_json_section<graphics_pipeline::sections::dynamic_states>( json, create_info.pipeline_json_ );

        const auto dynamic_state = vk::PipelineDynamicStateCreateInfo{ }
            .setDynamicStateCount( dynamic_states_data.size( ) )
            .setPDynamicStates( dynamic_states_data.data( ) );
        
        const auto input_assembly_state = parse_json_section<graphics_pipeline::sections::input_assembly>( json, create_info.pipeline_json_ );
        const auto rasterization_state = parse_json_section<graphics_pipeline::sections::rasterization>( json, create_info.pipeline_json_ );
        const auto multisampling_state = parse_json_section<graphics_pipeline::sections::multisampling>( json, create_info.pipeline_json_ );
        const auto colour_blend_attachments = parse_json_section<graphics_pipeline::sections::colour_blend_attachments>( json, create_info.pipeline_json_ );
        
        
        auto colour_blend_state = parse_json_section<graphics_pipeline::sections::colour_blend>( json, create_info.pipeline_json_ );
        colour_blend_state.attachmentCount = colour_blend_attachments.size( );
        colour_blend_state.pAttachments = colour_blend_attachments.data( );

        // TEMP
        const auto pipeline_layout_create_info = vk::PipelineLayoutCreateInfo{ };
        layout_ = create_info.device_.createPipelineLayoutUnique( pipeline_layout_create_info );
        //

        const auto pipeline_create_info = vk::GraphicsPipelineCreateInfo{ }
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
            .setPDynamicState( &dynamic_state )
            .setLayout( layout_.get() )
            .setRenderPass( create_info.render_pass_ )
            .setSubpass( 0 )
            .setBasePipelineHandle( nullptr )
            .setBasePipelineIndex( -1 );

        auto test = create_info.device_.createGraphicsPipelinesUnique( { }, pipeline_create_info );
                
        pipeline_ = std::move( test[0] );
    }
    graphics_pipeline::pipeline( graphics_pipeline&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }

    graphics_pipeline& graphics_pipeline::operator=( graphics_pipeline&& rhs ) noexcept
    {
        if ( this != &rhs )
        {
            pipeline_ = std::move( rhs.pipeline_ );
            layout_ = std::move( rhs.layout_ );

            vert_shader_id_ = rhs.vert_shader_id_;
            frag_shader_id_ = rhs.frag_shader_id_;
        }

        return *this;
    }

    void graphics_pipeline::bind( const vk::CommandBuffer& command_buffer ) const noexcept
    {
        command_buffer.bindPipeline( vk::PipelineBindPoint::eGraphics, pipeline_.get( ) );
    }

    void graphics_pipeline::set_viewport( const vk::CommandBuffer& command_buffer, const std::uint32_t first, const std::vector<vk::Viewport>& viewports ) const
    {
        command_buffer.setViewport( first, viewports );
    }
    void graphics_pipeline::set_scissors( const vk::CommandBuffer& command_buffer, const std::uint32_t first, const std::vector<vk::Rect2D>& scissors ) const
    {
        command_buffer.setScissor( first, scissors );
    }  
    void graphics_pipeline::set_line_width( const vk::CommandBuffer& command_buffer, const float width ) const
    {
        command_buffer.setLineWidth( width );
    }

} // marsupial::vulkan
