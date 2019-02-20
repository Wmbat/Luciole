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

#ifndef MARSUPIAL_VULKAN_PIPELINE_HPP
#define MARSUPIAL_VULKAN_PIPELINE_HPP

#include <unordered_map>
#include <string_view>

#include <nlohmann/json.hpp>

#include "shader_manager.hpp"
#include "../marsupial_core.hpp"
#include "../graphics/vertex.hpp"
#include "../utilities/file_io.hpp"
#include "../utilities/log.hpp"

namespace marsupial::vulkan
{
    enum class pipeline_type
    {
        graphics,
        compute 
    };

    /*!
     * 
     */
    template<auto layout_type>
    struct json_layout;

    template<>
    struct json_layout<pipeline_type::graphics>
    {
        enum class sections
        {
            input_assembly,
            rasterization,
            multisampling,
            colour_blend_attachments,
            colour_blend,
            dynamic_states,
        };  
        enum class values
        {
            boolean,
            topology,
            cull_mode,
            polygon_mode,
            front_face,
            blend_factor,
            blend_op,
            logic_op,
            dynamic_state
        };  
        enum class dynamic_states
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
        inline static const std::unordered_map<const char*, const vk::Bool32> bool_values_ {
            { "true", true },
            { "false", false }
        };  
        inline static const std::unordered_map<const char*, const vk::PrimitiveTopology> supported_topologies_ {
            { "triangle_list", vk::PrimitiveTopology::eTriangleList },
            { "triangle_list_with_adjacency", vk::PrimitiveTopology::eTriangleListWithAdjacency },
            { "triangle_strip", vk::PrimitiveTopology::eTriangleStrip },
            { "triangle_strip_with_adjacency", vk::PrimitiveTopology::eTriangleStripWithAdjacency },
            { "triangle_fan", vk::PrimitiveTopology::eTriangleFan },
            { "line_list", vk::PrimitiveTopology::eLineList },
            { "line_list_with_adjacency", vk::PrimitiveTopology::eLineListWithAdjacency },
            { "line_strip", vk::PrimitiveTopology::eLineStrip },
            { "line_strip_with_adjacency", vk::PrimitiveTopology::eLineStrip },
            { "patch_list", vk::PrimitiveTopology::ePatchList },
            { "point_list", vk::PrimitiveTopology::ePointList }
        };  
        inline static const std::unordered_map<const char*, const vk::PolygonMode> supported_polygon_modes_ {
            { "point", vk::PolygonMode::ePoint },
            { "line", vk::PolygonMode::eLine },
            { "fill", vk::PolygonMode::eFill }
        };  
        inline static const std::unordered_map<const char*, const vk::CullModeFlagBits> supported_cull_modes_ {
            { "none", vk::CullModeFlagBits::eNone }, 
            { "back", vk::CullModeFlagBits::eBack }, 
            { "front", vk::CullModeFlagBits::eFront }, 
            { "front_and_back", vk::CullModeFlagBits::eFrontAndBack }
        };  
        inline static const std::unordered_map<const char*, const vk::FrontFace> supported_front_faces_ {
            { "clockwise", vk::FrontFace::eClockwise },
            { "counter_clockwise", vk::FrontFace::eCounterClockwise }
        };  
        inline static const std::unordered_map<const char*, const vk::BlendFactor> supported_blend_factors_ {
            { "zero", vk::BlendFactor::eZero },
            { "one", vk::BlendFactor::eOne },
            { "src_colour", vk::BlendFactor::eSrcColor },
            { "one_minus_src_colour", vk::BlendFactor::eOneMinusSrcColor },
            { "dst_colour", vk::BlendFactor::eDstColor },
            { "one_minus_dst_colour", vk::BlendFactor::eOneMinusDstColor },
            { "src_alpha", vk::BlendFactor::eSrcAlpha },
            { "one_minus_src_alpha", vk::BlendFactor::eOneMinusSrcAlpha },
            { "dst_alpha", vk::BlendFactor::eDstAlpha },
            { "one_minus_dst_alpha", vk::BlendFactor::eOneMinusDstAlpha },
            { "constant_colour", vk::BlendFactor::eConstantColor },
            { "one_minus_constant_colour", vk::BlendFactor::eOneMinusConstantColor },
            { "constant_alpha", vk::BlendFactor::eConstantAlpha },
            { "one_minus_constant_alpha", vk::BlendFactor::eOneMinusConstantAlpha },
            { "src_alpha_saturate", vk::BlendFactor::eSrcAlphaSaturate },
            { "src_one_colour", vk::BlendFactor::eSrc1Color },
            { "one_minus_src_one_colour", vk::BlendFactor::eOneMinusSrc1Color },
            { "src_one_alpha", vk::BlendFactor::eSrc1Alpha },
            { "one_minus_src_one_alpha", vk::BlendFactor::eOneMinusSrc1Alpha }
        };  
        inline static const std::unordered_map<const char*, const vk::BlendOp> supported_blend_ops_ {
            { "add", vk::BlendOp::eAdd },
            { "substract", vk::BlendOp::eSubtract },
            { "reverse_substract", vk::BlendOp::eReverseSubtract },
            { "min", vk::BlendOp::eMin },
            { "max", vk::BlendOp::eMax }
        };  
        inline static const std::unordered_map<const char*, const vk::LogicOp> supported_logic_ops_ {
            { "clear", vk::LogicOp::eClear },
            { "and", vk::LogicOp::eAnd },
            { "and_reverse", vk::LogicOp::eAndReverse },
            { "copy", vk::LogicOp::eCopy },
            { "and_inverted", vk::LogicOp::eAndInverted },
            { "no_op", vk::LogicOp::eNoOp },
            { "xor", vk::LogicOp::eXor },
            { "or", vk::LogicOp::eOr },
            { "equivalent",vk::LogicOp::eEquivalent },
            { "invert", vk::LogicOp::eInvert },
            { "or_reverse", vk::LogicOp::eOrReverse },
            { "copy_inverted", vk::LogicOp::eCopyInverted },
            { "or_inverted", vk::LogicOp::eOrInverted },
            { "nand", vk::LogicOp::eNand },
            { "set", vk::LogicOp::eSet }
        };  
        inline static const std::unordered_map<const char*, const vk::DynamicState> supported_dynamic_states_ {
            { "viewport", vk::DynamicState::eViewport },
            { "scissor", vk::DynamicState::eScissor },
            { "line_width", vk::DynamicState::eLineWidth }
        };
    };

    using graphics_pipeline_json_layout = json_layout<pipeline_type::graphics>;
    using graphics_pipeline_section = graphics_pipeline_json_layout::sections;

    /*!
     * Default template specialization for json traits
     */
    template<auto json_type>
    struct pipeline_trait;

    template<>
    struct pipeline_trait<graphics_pipeline_section::input_assembly>
    {
        using type = vk::PipelineInputAssemblyStateCreateInfo;
    };

    template<>
    struct pipeline_trait<graphics_pipeline_section::rasterization>
    {
        using type = vk::PipelineRasterizationStateCreateInfo;
    };

    template<>
    struct pipeline_trait<graphics_pipeline_section::multisampling>
    {
        using type = vk::PipelineMultisampleStateCreateInfo;
    };

    template<>
    struct pipeline_trait<graphics_pipeline_section::colour_blend_attachments>
    {
        using type = std::vector<vk::PipelineColorBlendAttachmentState>;
    };

    template<>
    struct pipeline_trait<graphics_pipeline_section::colour_blend>
    {
        using type = vk::PipelineColorBlendStateCreateInfo; 
    }; 

    template<>
    struct pipeline_trait<graphics_pipeline_section::dynamic_states>
    {
        using type = std::vector<vk::DynamicState>;
    };

    template<auto json_type>
    using json_return_t = typename pipeline_trait<json_type>::type;


    /*!
     *
     */
    template<auto type>
    class pipeline;

    template<>
    class pipeline<pipeline_type::graphics>
    {
    public:
        struct create_info
        {
            create_info& set_device( const vk::Device device )
            {
                device_ = device;
                return *this;
            }
            create_info& set_render_pass( const vk::RenderPass render_pass )
            {
                render_pass_ = render_pass;
                return *this;
            }
            create_info& set_pipeline_definition( const std::string& pipeline_json )
            {
                pipeline_json_ = pipeline_json;
                return *this;
            }
            create_info& set_viewports( const std::vector<vk::Viewport>& viewports )
            {
                viewports_ = viewports;
                return *this;
            }
            create_info& set_scissors( const std::vector<vk::Rect2D>& scissors )
            {
                scissors_ = scissors;
                return *this;
            }
            create_info& set_shader_manager( shader_manager* p_shader_manager )
            {
                p_shader_manager_ = p_shader_manager;
                return *this;
            }
            create_info& set_shader_ids( uint32_t vert_shader_id, uint32_t frag_shader_id )
            {
                vert_shader_id_ = vert_shader_id;
                frag_shader_id_ = frag_shader_id;
                return *this;
            }

            vk::Device device_;
            vk::RenderPass render_pass_;

            std::string pipeline_json_;

            std::vector<vk::Viewport> viewports_;
            std::vector<vk::Rect2D> scissors_;

            shader_manager* p_shader_manager_;
            uint32_t vert_shader_id_;
            uint32_t frag_shader_id_;
        };

    public:
        pipeline( ) = default;
        pipeline( const create_info& create_info );
        pipeline( const pipeline& rhs ) = delete;
        pipeline( pipeline&& rhs ) noexcept
        {
            *this = std::move( rhs );
        }
        ~pipeline( ) = default;

        pipeline& operator=( const pipeline& rhs ) = delete;
        pipeline& operator=( pipeline&& rhs )
        {
            if ( this != &rhs )
            {
            }

            return *this;
        }

        void bind( const vk::CommandBuffer& command_buffer ) const noexcept
        {
            command_buffer.bindPipeline( vk::PipelineBindPoint::eGraphics, pipeline_.get( ) );
        }

        void set_viewport( const vk::CommandBuffer& command_buffer, const std::uint32_t first, const std::vector<vk::Viewport>& viewports ) const noexcept
        {
            if ( is_viewport_dynamic_ )
            {
                command_buffer.setViewport( first, viewports );
            }
        }

    private:
        template<auto json_section>
        const json_return_t<json_section> parse_json_section_t( const nlohmann::json& json, const char* pipeline_name ) const;

        template<auto json_value>
        const json_return_t<json_value> parse_json_value_t( const std::string& value, const char* pipeline_name, const char* location ) const
        {

        }

    private:
        vk::UniquePipeline pipeline_;

        std::uint32_t vert_shader_id_;
        std::uint32_t frag_shader_id_;

        graphics_pipeline_json_layout json_layout_;

        bool is_viewport_dynamic_ = false;
        bool is_scissor_dynamic_ = false;
        bool is_line_width_dynamic_ = false;
    };

    using graphics_pipeline = pipeline<pipeline_type::graphics>;

    /*!
     * Specialization of pipeline<pipeline_type::graphics>'s function "parse_json_value".
     */

    template<>
    const json_return_t<graphics_pipeline_section::input_assembly>
    graphics_pipeline::parse_json_section_t<graphics_pipeline_section::input_assembly>( const nlohmann::json& json, const char* pipeline_name ) const
    {   
        //const auto section = json["input_assembly"];

        const auto create_info = vk::PipelineInputAssemblyStateCreateInfo{ };
        /*
            .setPrimitiveRestartEnable( parse_json_value<pipeline_json_layout::values::boolean>( 
                section["primitive_restart_enable"], pipeline_name, "\"input_assembly\"/\"primitive_restart_enable\"" ) )
            .setTopology( parse_json_value<pipeline_json_layout::values::topology>(
                section["topology"], pipeline_name, "\"input_assembly\"/\"topology\"" ) );
        */

        return create_info;
    }

    template<>
    const json_return_t<graphics_pipeline_section::rasterization>
    graphics_pipeline::parse_json_section_t<graphics_pipeline_section::rasterization>( const nlohmann::json& json, const char* pipeline_name ) const
    {

    }

    template<>
    const json_return_t<graphics_pipeline_section::multisampling>
    graphics_pipeline::parse_json_section_t<graphics_pipeline_section::multisampling>( const nlohmann::json& json, const char* pipeline_name ) const
    {

    }

    template<>
    const json_return_t<graphics_pipeline_section::colour_blend_attachments>
    graphics_pipeline::parse_json_section_t<graphics_pipeline_section::colour_blend_attachments>( const nlohmann::json& json, const char* pipeline_name ) const
    {
        
    }

    template<>
    const json_return_t<graphics_pipeline_section::colour_blend>
    graphics_pipeline::parse_json_section_t<graphics_pipeline_section::colour_blend>( const nlohmann::json& json, const char* pipeline_name ) const
    {

    }

    template<>
    const json_return_t<graphics_pipeline_section::dynamic_states>
    graphics_pipeline::parse_json_section_t<graphics_pipeline_section::dynamic_states>( const nlohmann::json& json, const char* pipeline_name ) const
    {

    }

    graphics_pipeline::pipeline( const graphics_pipeline::create_info& create_info )
        :
        vert_shader_id_( create_info.vert_shader_id_ ),
        frag_shader_id_( create_info.frag_shader_id_ )
    {
        const auto json = nlohmann::json::parse( read_from_file( create_info.pipeline_json_ ) );

        const vk::PipelineShaderStageCreateInfo shader_stage_create_infos[] = {
            create_info.p_shader_manager_->find<shader_type::vertex>( vert_shader_id_ ).get_shader_stage_create_info( ),
            create_info.p_shader_manager_->find<shader_type::fragment>( frag_shader_id_ ).get_shader_stage_create_info( )
        };

        const auto viewport_state = vk::PipelineViewportStateCreateInfo( )
            .setViewportCount( static_cast<uint32_t>( create_info.viewports_.size() ) )
            .setPViewports( create_info.viewports_.data() )
            .setScissorCount( static_cast<uint32_t>( create_info.scissors_.size() ) )
            .setPScissors( create_info.scissors_.data() );

        const auto input_assembly = parse_json_section_t<graphics_pipeline_section::input_assembly>( json, create_info.pipeline_json_.c_str( ) );
        const auto rasterization = parse_json_section_t<graphics_pipeline_section::rasterization>( json, create_info.pipeline_json_.c_str( ) );
        const auto multisampling = parse_json_section_t<graphics_pipeline_section::multisampling>( json, create_info.pipeline_json_.c_str( ) );
        const auto colour_blend_attachments = parse_json_section_t<graphics_pipeline_section::colour_blend_attachments>( json, create_info.pipeline_json_.c_str( ) );
        

        const auto pipeline_create_info = vk::GraphicsPipelineCreateInfo{ };
    }

/*
    template<>
    const auto graphics_pipeline::parse_json_value<pipeline_json_layout::values::boolean>(
        const std::string& value, const char* pipeline_name, const char* location ) const
    {
        const auto it = json_layout_.bool_values_.find( value.c_str( ) );

        if ( it == json_layout_.bool_values_.cend( ) )
        {
            core_warn( "Error parsing pipeline data from \"{0}\". \"{1}\" is not a valid parameter for {2}. Please "
                "refer to documentation regarding supported cull modes.", pipeline_name , value, location );

            return static_cast<vk::Bool32>( false );
        }

        return it->second;
    }

    template<>
    const auto graphics_pipeline::parse_json_value<pipeline_json_layout::values::topology>( 
        const std::string& value, const char* pipeline_name, const char* location ) const
    {
        const auto it = json_layout_.supported_topologies_.find( value.c_str( ) );

        if ( it == json_layout_.supported_topologies_.cend( ) )
        {
            core_warn( "Error parsing pipeline data from \"{0}\". \"{1}\" is not a valid parameter for {2}. Please "
                "refer to documentation regarding supported cull modes.", pipeline_name , value, location );

            return vk::PrimitiveTopology::eTriangleStrip;
        }

        return it->second;
    }

    template<>
    const auto graphics_pipeline::parse_json_value<pipeline_json_layout::values::cull_mode>(
        const std::string& value, const char* pipeline_name, const char* location ) const
    {
        const auto it = json_layout_.supported_cull_modes_.find( value.c_str( ) );

        if ( it == json_layout_.supported_cull_modes_.cend( ) )
        {
            core_warn( "Error parsing pipeline data from \"{0}\". \"{1}\" is not a valid parameter for {2}. Please "
                "refer to documentation regarding supported cull modes.", pipeline_name , value, location );

            return vk::CullModeFlagBits::eBack;
        }

        return it->second;
    }

    template<>
    const auto graphics_pipeline::parse_json_value<pipeline_json_layout::values::polygon_mode>(
        const std::string& value, const char* pipeline_name, const char* location ) const
    {
        const auto it = json_layout_.supported_polygon_modes_.find( value.c_str( ) );

        if ( it == json_layout_.supported_polygon_modes_.cend( ) )
        {
            core_warn( "Error parsing pipeline data from \"{0}\". \"{1}\" is not a valid parameter for {2}. Please "
                "refer to documentation regarding supported cull modes.", pipeline_name , value, location );

            return vk::PolygonMode::eFill;
        }

        return it->second;
    }

    template<>
    const auto graphics_pipeline::parse_json_value<pipeline_json_layout::values::front_face>( 
        const std::string& value, const char* pipeline_name, const char* location ) const
    {
        const auto it = json_layout_.supported_front_faces_.find( value.c_str( ) );

        if ( it == json_layout_.supported_front_faces_.cend( ) )
        {
            core_warn( "Error parsing pipeline data from \"{0}\". \"{1}\" is not a valid parameter for {2}. Please "
                "refer to documentation regarding supported cull modes.", pipeline_name , value, location );

            return vk::FrontFace::eClockwise;
        }

        return it->second;
    }

    template<>
    const auto graphics_pipeline::parse_json_value<pipeline_json_layout::values::blend_factor>(
        const std::string& value, const char* pipeline_name, const char* location ) const
    {
        const auto it = json_layout_.supported_blend_factors_.find( value.c_str( ) );

        if ( it == json_layout_.supported_blend_factors_.cend( ) )
        {
            core_warn( "Error parsing pipeline data from \"{0}\". \"{1}\" is not a valid parameter for {2}. Please "
                "refer to documentation regarding supported cull modes.", pipeline_name , value, location );
        
            return vk::BlendFactor::eZero;
        }

        return it->second;
    }
    */


    /*
    template<>
    const graphics_pipeline::section_return_type<pipeline_json_layout::sections::input_assembly> 
    graphics_pipeline::parse_json_section<pipeline_json_layout::sections::input_assembly>( const nlohmann::json& json, const char* pipeline_name ) const
    {
        const auto section = json["input_assembly"];

        const auto create_info = vk::PipelineInputAssemblyStateCreateInfo{ }
            .setPrimitiveRestartEnable( parse_json_value<pipeline_json_layout::values::boolean>( 
                section["primitive_restart_enable"], pipeline_name, "\"input_assembly\"/\"primitive_restart_enable\"" ) )
            .setTopology( parse_json_value<pipeline_json_layout::values::topology>(
                section["topology"], pipeline_name, "\"input_assembly\"/\"topology\"" ) );

        return json_section_trait;
    }

    template<>
    const vk::PipelineRasterizationStateCreateInfo 
    graphics_pipeline::parse_json_section<vk::PipelineRasterizationStateCreateInfo, pipeline_json_layout::sections::rasterization>(
        const nlohmann::json& json, const char* pipeline_name ) const
    {
        const auto section = json["rasterization"];
    
        const auto create_info = vk::PipelineRasterizationStateCreateInfo{ }
            .setDepthClampEnable( parse_json_value<pipeline_json_layout::values::boolean>( 
                section["depth_clamp_enable"], pipeline_name, "\"rasterization\"/\"depth_clamp_enable\"" ) )
            .setRasterizerDiscardEnable( parse_json_value<pipeline_json_layout::values::boolean>(
                section["rasteriazer_discard_enable"], pipeline_name, "\"rasterization\"/\"rasterizer_discard_enable\"" ) )
            .setPolygonMode( parse_json_value<pipeline_json_layout::values::polygon_mode>(
                section["polygon_mode"], pipeline_name, "\"rasterization\"/\"polygon_mode\"" ) )
            .setFrontFace( parse_json_value<pipeline_json_layout::values::front_face>(
                section["front_face"], pipeline_name, "\"rasterization\"/\"front_Face\"" ) )
            .setDepthBiasEnable( parse_json_value<pipeline_json_layout::values::boolean>(
                section["depth_bias_enable"], pipeline_name, "\"rasterization\"/\"depth_bias_enable\"" ) )
            .setDepthBiasConstantFactor( section["depth_bias_constant_factor"] )
            .setDepthBiasClamp( section["depth_bias_clamp"] )
            .setDepthBiasSlopeFactor( section["depth_bias_slope_factor"] )
            .setLineWidth( section["line_width"] );

        return create_info;
    }
*/
    template<>
    class pipeline<pipeline_type::compute>
    {
    public:

    private:
    };

    using compute_pipeline = pipeline<pipeline_type::compute>;


/*
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
                    .setStride( sizeof( glm::vec3 ) )
                    .setInputRate( vk::VertexInputRate::eVertex ) );

            vertex_input_config_.bindings_.emplace_back(
                vk::VertexInputBindingDescription( )
                    .setBinding( 1 )
                    .setStride( sizeof( glm::vec4 ) )
                    .setInputRate( vk::VertexInputRate::eVertex ) );
            
            vertex_input_config_.attributes_.emplace_back(
                vk::VertexInputAttributeDescription( )
                    .setBinding( 0 )
                    .setLocation( 0 )
                    .setFormat( vk::Format::eR32G32B32Sfloat )
                    .setOffset( 0 )
                );
            
            vertex_input_config_.attributes_.emplace_back(
                vk::VertexInputAttributeDescription( )
                    .setBinding( 1 )
                    .setLocation( 1 )
                    .setFormat( vk::Format::eR32G32B32A32Sfloat )
                    .setOffset( 0 )
                );
            //
            
            dynamic_states_ = get_dynamic_states( json["dynamic_states"] );
            
            const auto vertex_input_state = vk::PipelineVertexInputStateCreateInfo( )
                .setVertexBindingDescriptionCount( static_cast<uint32_t>( vertex_input_config_.bindings_.size() ) )
                .setPVertexBindingDescriptions( vertex_input_config_.bindings_.data() )
                .setVertexAttributeDescriptionCount( static_cast<uint32_t>( vertex_input_config_.attributes_.size() ) )
                .setPVertexAttributeDescriptions( vertex_input_config_.attributes_.data() );
            
            const auto input_assembly_state = parse_json_section<json_section::input_assembly>( json["input_assembly"], create_info.pipeline_json_ );

            const auto viewport_state = vk::PipelineViewportStateCreateInfo( )
                .setViewportCount( static_cast<uint32_t>( create_info.viewports_.size() ) )
                .setPViewports( create_info.viewports_.data() )
                .setScissorCount( static_cast<uint32_t>( create_info.scissors_.size() ) )
                .setPScissors( create_info.scissors_.data() );
            
            const auto rasterization_state = vk::PipelineRasterizationStateCreateInfo( )
                .setDepthClampEnable( 
                    parse_bool_value( 
                        json["rasterization"]["depth_clamp_enable"], 
                        create_info.pipeline_json_, 
                        "\"rasterization\":\"depth_clamp_enable\"" ) )
                .setRasterizerDiscardEnable( 
                    parse_bool_value( 
                        json["rasterization"]["rasterization_discard_enable"],
                        create_info.pipeline_json_,
                        "\"rasterization\":\"rasterization_discard_enable\"" ) )
                .setPolygonMode( get_polygon_mode( json["rasterization"]["polygon_mode"] ) )
                .setCullMode( parse_cull_mode_value( json["rasterization"]["cull_mode"], create_info.pipeline_json_ ) )
                .setFrontFace( get_front_face( json["rasterization"]["front_face"] ) )
                .setDepthBiasEnable( 
                    parse_bool_value( 
                        json["rasterization"]["depth_bias_enable"], 
                        create_info.pipeline_json_, 
                        "\"rasterization\":\"depth_bias_enable\"" ) )
                .setDepthBiasConstantFactor( json["rasterization"]["depth_bias_constant_factor"] )
                .setDepthBiasClamp( json["rasterization"]["depth_bias_clamp"] )
                .setDepthBiasSlopeFactor( json["rasterization"]["depth_bias_slope_factor"] )
                .setLineWidth( json["rasterization"]["line_width"] );
            
			const auto multisampling_state = vk::PipelineMultisampleStateCreateInfo( )
				.setRasterizationSamples( get_rasterization_samples( json["multisampling"]["rasterization_samples"] ) )
				.setSampleShadingEnable( 
                    parse_bool_value( 
                        json["multisampling"]["sample_shading_enable"],
                        create_info.pipeline_json_, 
                        "\"multisampling\":\"sample_shading_enable\"" ) )
				.setMinSampleShading( json["multisampling"]["min_sample_shading"])
				.setPSampleMask( nullptr )
				.setAlphaToCoverageEnable( 
                    parse_bool_value( 
                        json["multisampling"]["alpha_to_coverage_enable"],
                        create_info.pipeline_json_, 
                        "\"multisampling\":\"alpha_to_coverage_enable\"" ) )
                .setAlphaToOneEnable( 
                    parse_bool_value( 
                        json["multisampling"]["alpha_to_one_enable"],
                        create_info.pipeline_json_, 
                        "\"multisampling\":\"alpha_to_one_enable\"" ) );
            
            const auto colour_blend_attachment_states = parse_json_section<json_section::colour_blend_attachments>( json["colour_blend_attachments"], create_info.pipeline_json_ );

            const auto colour_blend_state = vk::PipelineColorBlendStateCreateInfo( )
                .setLogicOpEnable( 
                    parse_bool_value( 
                        json["colour_blend"]["logic_op_enable"],
                        create_info.pipeline_json_, 
                        "\"colour_blend\":\"logic_op_enable\"" ) )
                .setLogicOp( get_logic_op( json["colour_blend"]["logic_op"] ) )
                .setAttachmentCount( static_cast<uint32_t>( colour_blend_attachment_states.size() ) )
                .setPAttachments( colour_blend_attachment_states.data() )
                .setBlendConstants( 
                    std::array<float, 4>{
                        json["colour_blend"]["blend_constants"][0],
                        json["colour_blend"]["blend_constants"][1],
                        json["colour_blend"]["blend_constants"][2],
                        json["colour_blend"]["blend_constants"][3] } );
            
            const auto dynamic_state_create_info = vk::PipelineDynamicStateCreateInfo( )
                .setDynamicStateCount( static_cast<uint32_t>( dynamic_states_.size() ) )
                .setPDynamicStates( dynamic_states_.data() );
            
            const auto layout_create_info = vk::PipelineLayoutCreateInfo( );
            
            layout_ = create_info.device_.createPipelineLayoutUnique( layout_create_info );
            
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
                    .setRenderPass( create_info.render_pass_ )
                    .setSubpass( 0 )
                    .setBasePipelineHandle( nullptr )
                    .setBasePipelineIndex( -1 );
                
                auto test = create_info.device_.createGraphicsPipelinesUnique( nullptr, graphics_create_info );
                
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
        template<json_value value>
        const auto parse_json_value( const std::string_view& json_value, const std::string_view& pipeline_name, const std::string_view& location ) const
        {
            core_warn( "Error: parse_json_value sinkhole function." );
        }


        const vk::Bool32 parse_bool_value( const std::string_view& bool_value, const std::string_view& pipeline_name, const std::string_view& location ) const
        {
            const auto it = bool_values.find( bool_value );

            if ( it == bool_values.cend( ) )
            {
                core_warn( "Error parsing pipeline data from \"{0}\". \"{1}\" is not a valid bool value for {2}.", pipeline_name , bool_value, location );

                return false;
            }

            return it->second;
        }

        const vk::CullModeFlagBits parse_cull_mode_value( const std::string_view& cull_mode, const std::string_view& pipeline_name ) const
        {
            const auto it = supported_cull_modes.find( cull_mode );

            if ( it == supported_cull_modes.cend( ) )
            {
                core_warn( "Error parsing pipeline data from \"{0}\". \"{1}\" is not a valid parameter for \"rasterization\":\"cull_mode\". Please "
                    "refer to documentation regarding supported cull modes.", pipeline_name , cull_mode );

                return vk::CullModeFlagBits::eNone;
            }

            return it->second;
        }

        template<json_section section, typename = std::enable_if_t<section == json_section::input_assembly>>
        const vk::PipelineInputAssemblyStateCreateInfo parse_json_section( const nlohmann::json& section_data, const std::string_view& pipeline_name ) const
        {
            
            auto create_info = vk::PipelineInputAssemblyStateCreateInfo{ };
                .setPrimitiveRestartEnable( 
                    parse_bool_value( 
                        section_data["primitive_restart_enable"], 
                        pipeline_name, 
                        "\"input_assembly\":\"primitive_restart_enable\"" ) )
                .setTopology( 
                    parse_topology_value(
                        section_data["topology"],
                        pipeline_name,
                        "\"input_assembly\":\"topology\"" ) );
            return create_info;
        }

        template<json_section section, typename = std::enable_if_t<section == json_section::colour_blend_attachments>>
        const std::vector<vk::PipelineColorBlendAttachmentState> parse_json_section( const nlohmann::json& section_data, const string_view& pipeline_name ) const
        {
            std::vector<vk::PipelineColorBlendAttachmentState> states;

            for( const auto& data : json )
            {

                const auto colour_blend_attachment = vk::PipelineColorBlendAttachmentState( )
                    .setBlendEnable( 
                            parse_bool_value( 
                                data["blend_enable"], 
                                pipeline_name, 
                                "\"colour_blend_attachment\":\"blend_enable\"" ) )
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
        }

        template<json_section section, typename = std::enable_if_t<section == json_section::rasterization>>
        const vk::PipelineRasterizationStateCreateInfo parse_json_section( const nlohmann::json& section_data, const std::string_view& pipeline_name ) const
        {
            return vk::PipelineRasterizationStateCreateInfo{ };
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

        inline static std::unordered_map<std::string_view, vk::Bool32> bool_values {
            { "true", true },
            { "false", false }
        };

        inline static std::unordered_map<std::string_view, vk::PrimitiveTopology> supported_topologies {
            { "triangle_list", vk::PrimitiveTopology::eTriangleList },
            { "triangle_list_with_adjacency", vk::PrimitiveTopology::eTriangleListWithAdjacency },
            { "triangle_strip", vk::PrimitiveTopology::eTriangleStrip },
            { "triangle_strip_with_adjacency", vk::PrimitiveTopology::eTriangleStripWithAdjacency },
            { "triangle_fan", vk::PrimitiveTopology::eTriangleFan },
            { "line_list", vk::PrimitiveTopology::eLineList },
            { "line_list_with_adjacency", vk::PrimitiveTopology::eLineListWithAdjacency },
            { "line_strip", vk::PrimitiveTopology::eLineStrip },
            { "line_strip_with_adjacency", vk::PrimitiveTopology::eLineStrip },
            { "patch_list", vk::PrimitiveTopology::ePatchList },
            { "point_list", vk::PrimitiveTopology::ePointList }
        };

        inline static std::unordered_map<std::string_view, vk::CullModeFlagBits> supported_cull_modes {
            { "none", vk::CullModeFlagBits::eNone }, 
            { "back", vk::CullModeFlagBits::eBack }, 
            { "front", vk::CullModeFlagBits::eFront }, 
            { "front_and_back", vk::CullModeFlagBits::eFrontAndBack }
        };
    };

    using graphics_pipeline = pipeline<pipeline_type::graphics>;
    using compute_pipeline = pipeline<pipeline_type::compute>;
    */
}

#endif //MARSUPIAL_VULKAN_PIPELINE_HPP
