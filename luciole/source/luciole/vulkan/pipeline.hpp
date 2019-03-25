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

#ifndef LUCIOLE_VULKAN_PIPELINE_HPP
#define LUCIOLE_VULKAN_PIPELINE_HPP

#include <unordered_map>
#include <string_view>

#include <nlohmann/json.hpp>

#include "shader_manager.hpp"

#include "../luciole_core.hpp"
#include "../graphics/vertex.hpp"
#include "../utilities/file_io.hpp"
#include "../utilities/log.hpp"

namespace lcl::vulkan
{
    enum class pipeline_type
    {
        graphics,
        compute 
    };

    template<auto json_type>
    struct json_trait;

    template<auto json_type>
    using json_return_t = typename json_trait<json_type>::type;

    template<auto type>
    class pipeline;

    template<>
    class pipeline<pipeline_type::graphics>
    {
    public:
        struct create_info
        {
            create_info& set_device( const vk::Device device ) noexcept
            {
                device_ = device;
                return *this;
            }
            create_info& set_render_pass( const vk::RenderPass render_pass ) noexcept
            {
                render_pass_ = render_pass;
                return *this;
            }
            create_info& set_pipeline_definition( const std::string& pipeline_json ) noexcept
            {
                pipeline_json_ = pipeline_json;
                return *this;
            }
            create_info& set_viewports( const std::vector<vk::Viewport>& viewports ) noexcept
            {
                viewports_ = viewports;
                return *this;
            }
            create_info& set_scissors( const std::vector<vk::Rect2D>& scissors ) noexcept
            {
                scissors_ = scissors;
                return *this;
            }
            create_info& set_shader_manager( shader_manager* p_shader_manager ) noexcept
            {
                p_shader_manager_ = p_shader_manager;
                return *this;
            }
            create_info& set_shader_ids( uint32_t vert_shader_id, uint32_t frag_shader_id ) noexcept
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
        pipeline( vk::Pipeline&& pipeline );
        pipeline( const pipeline& rhs ) = delete;
        pipeline( pipeline&& rhs ) noexcept;
        ~pipeline( ) = default;

        pipeline& operator=( const pipeline& rhs ) = delete;
        pipeline& operator=( pipeline&& rhs ) noexcept;

        void bind( const vk::CommandBuffer& command_buffer ) const noexcept;

        void set_viewport( const vk::CommandBuffer& command_buffer, const std::uint32_t first, const std::vector<vk::Viewport>& viewports ) const;
        void set_scissors( const vk::CommandBuffer& command_buffer, const std::uint32_t first, const std::vector<vk::Rect2D>& scissors ) const;
        void set_line_width( const vk::CommandBuffer& command_buffer, const float width ) const;

    private:
        template<auto json_value>
        const json_return_t<json_value> parse_json_value( const nlohmann::json& json, const std::string_view value_name, const std::string_view pipeline_name ) const;

        template<auto json_section>
        const json_return_t<json_section> parse_json_section( const nlohmann::json& json, const std::string& pipeline_name ) const;

    private:
        vk::UniquePipeline pipeline_;
        vk::UniquePipelineLayout layout_;

        std::uint32_t vert_shader_id_;
        std::uint32_t frag_shader_id_;

        using umap_bool_storage_t = std::unordered_map<std::string, const vk::Bool32>;
        using umap_topology_storage_t = std::unordered_map<std::string, const vk::PrimitiveTopology>;
        using umap_poly_mode_storage_t = std::unordered_map<std::string, const vk::PolygonMode>;

        inline static const umap_bool_storage_t bool_values_ 
        {
            { "true", true },
            { "false", false }
        };  
        inline static const umap_topology_storage_t supported_topologies_ 
        {
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
        inline static const umap_poly_mode_storage_t supported_polygon_modes_ 
        {
            { "point", vk::PolygonMode::ePoint },
            { "line", vk::PolygonMode::eLine },
            { "fill", vk::PolygonMode::eFill }
        };  
        inline static const std::unordered_map<std::string, const vk::CullModeFlagBits> supported_cull_modes_ 
        {
            { "none", vk::CullModeFlagBits::eNone }, 
            { "back", vk::CullModeFlagBits::eBack }, 
            { "front", vk::CullModeFlagBits::eFront }, 
            { "front_and_back", vk::CullModeFlagBits::eFrontAndBack }
        };  
        inline static const std::unordered_map<std::string, const vk::FrontFace> supported_front_faces_ 
        {
            { "clockwise", vk::FrontFace::eClockwise },
            { "counter_clockwise", vk::FrontFace::eCounterClockwise }
        };  
        inline static const std::unordered_map<std::uint32_t, const vk::SampleCountFlagBits> supported_sample_counts_ 
        {
            { 1, vk::SampleCountFlagBits::e1 },
            { 2, vk::SampleCountFlagBits::e2 },
            { 4, vk::SampleCountFlagBits::e4 },
            { 8, vk::SampleCountFlagBits::e8 },
            { 16, vk::SampleCountFlagBits::e16 },
            { 32, vk::SampleCountFlagBits::e32 },
            { 64, vk::SampleCountFlagBits::e64 }
        };
        inline static const std::unordered_map<std::string, const vk::BlendFactor> supported_blend_factors_ 
        {
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
        inline static const std::unordered_map<std::string, const vk::BlendOp> supported_blend_ops_ 
        {
            { "add", vk::BlendOp::eAdd },
            { "substract", vk::BlendOp::eSubtract },
            { "reverse_substract", vk::BlendOp::eReverseSubtract },
            { "min", vk::BlendOp::eMin },
            { "max", vk::BlendOp::eMax }
        };  
        inline static const std::unordered_map<std::string, const vk::LogicOp> supported_logic_ops_ 
        {
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
        inline static const std::unordered_map<std::string, const vk::DynamicState> supported_dynamic_states_ 
        {
            { "viewport", vk::DynamicState::eViewport },
            { "scissor", vk::DynamicState::eScissor },
            { "line_width", vk::DynamicState::eLineWidth }
        };
        inline static const std::unordered_map<std::string, const vk::ColorComponentFlagBits> supported_colour_components_
        {
            { "A", vk::ColorComponentFlagBits::eA },
            { "R", vk::ColorComponentFlagBits::eR },
            { "G", vk::ColorComponentFlagBits::eG },
            { "B", vk::ColorComponentFlagBits::eB }
        };

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
            bool32,
            uint32,
            floating_point,
            topology,
            cull_mode,
            polygon_mode,
            front_face,
            sample_count,
            blend_factor,
            blend_op,
            logic_op,
            colour_components,
            blend_constants
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
    };

    using graphics_pipeline = pipeline<pipeline_type::graphics>;

    /*!
     *
     */

    template<>
    struct json_trait<graphics_pipeline::values::bool32>
    {
        using type = vk::Bool32;  
    };

    template<>
    struct json_trait<graphics_pipeline::values::uint32>
    {
        using type = std::uint32_t;
    };

    template<>
    struct json_trait<graphics_pipeline::values::floating_point>
    {
        using type = float;
    };

    template<>
    struct json_trait<graphics_pipeline::values::topology>
    {
        using type = vk::PrimitiveTopology;
    };

    template<>
    struct json_trait<graphics_pipeline::values::cull_mode>
    {
        using type = vk::CullModeFlagBits;
    };

    template<>
    struct json_trait<graphics_pipeline::values::polygon_mode>
    {
        using type = vk::PolygonMode;
    };

    template<>
    struct json_trait<graphics_pipeline::values::front_face>
    {
        using type = vk::FrontFace;
    };

    template<>
    struct json_trait<graphics_pipeline::values::sample_count>
    {
        using type = vk::SampleCountFlagBits;
    };

    template<>
    struct json_trait<graphics_pipeline::values::blend_factor>
    {
        using type = vk::BlendFactor;
    };

    template<>
    struct json_trait<graphics_pipeline::values::blend_op>
    {
        using type = vk::BlendOp;
    };

    template<>
    struct json_trait<graphics_pipeline::values::logic_op>
    {
        using type = vk::LogicOp;
    };

    template<>
    struct json_trait<graphics_pipeline::values::colour_components>
    {
        using type = vk::ColorComponentFlags;
    };

    template<>
    struct json_trait<graphics_pipeline::values::blend_constants>
    {
        using type = std::array<float, 4>;
    };

    /*!
     * 
     */
    template<>
    struct json_trait<graphics_pipeline::sections::input_assembly>
    {
        using type = vk::PipelineInputAssemblyStateCreateInfo;
    };

    template<>
    struct json_trait<graphics_pipeline::sections::rasterization>
    {
        using type = vk::PipelineRasterizationStateCreateInfo;
    };

    template<>
    struct json_trait<graphics_pipeline::sections::multisampling>
    {
        using type = vk::PipelineMultisampleStateCreateInfo;
    };

    template<>
    struct json_trait<graphics_pipeline::sections::colour_blend_attachments>
    {
        using type = std::vector<vk::PipelineColorBlendAttachmentState>;
    };

    template<>
    struct json_trait<graphics_pipeline::sections::colour_blend>
    {
        using type = vk::PipelineColorBlendStateCreateInfo; 
    }; 

    template<>
    struct json_trait<graphics_pipeline::sections::dynamic_states>
    {
        using type = std::vector<vk::DynamicState>;
    };
}

#endif //LUCIOLE_VULKAN_PIPELINE_HPP
