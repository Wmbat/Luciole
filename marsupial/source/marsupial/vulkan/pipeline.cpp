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

namespace marsupial::vulkan
{
    template<>
    const json_return_t<graphics_pipeline::sections::input_assembly>
    graphics_pipeline::parse_json_section_t<graphics_pipeline::sections::input_assembly>( const nlohmann::json& json, const char* pipeline_name ) const
    {

    }

    template<>
    const json_return_t<graphics_pipeline::sections::rasterization>
    graphics_pipeline::parse_json_section_t<graphics_pipeline::sections::rasterization>( const nlohmann::json& json, const char* pipeline_name ) const
    {

    }

    template<>
    const json_return_t<graphics_pipeline::sections::multisampling>
    graphics_pipeline::parse_json_section_t<graphics_pipeline::sections::multisampling>( const nlohmann::json& json, const char* pipeline_name ) const
    {

    }

    template<>
    const json_return_t<graphics_pipeline::sections::colour_blend_attachments>
    graphics_pipeline::parse_json_section_t<graphics_pipeline::sections::colour_blend_attachments>( const nlohmann::json& json, const char* pipeline_name ) const
    {
        
    }

    template<>
    const json_return_t<graphics_pipeline::sections::colour_blend>
    graphics_pipeline::parse_json_section_t<graphics_pipeline::sections::colour_blend>( const nlohmann::json& json, const char* pipeline_name ) const
    {

    }

    template<>
    const json_return_t<graphics_pipeline::sections::dynamic_states>
    graphics_pipeline::parse_json_section_t<graphics_pipeline::sections::dynamic_states>( const nlohmann::json& json, const char* pipeline_name ) const
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

        const auto input_assembly = parse_json_section_t<graphics_pipeline::sections::input_assembly>( json, create_info.pipeline_json_.c_str( ) );
        const auto rasterization = parse_json_section_t<graphics_pipeline::sections::rasterization>( json, create_info.pipeline_json_.c_str( ) );
        const auto multisampling = parse_json_section_t<graphics_pipeline::sections::multisampling>( json, create_info.pipeline_json_.c_str( ) );
        const auto colour_blend_attachments = parse_json_section_t<graphics_pipeline::sections::colour_blend_attachments>( json, create_info.pipeline_json_.c_str( ) );

        const auto pipeline_create_info = vk::GraphicsPipelineCreateInfo{ };
    }
    graphics_pipeline::pipeline( graphics_pipeline&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }

    graphics_pipeline& graphics_pipeline::operator=( graphics_pipeline&& rhs ) noexcept
    {
        if ( this != &rhs )
        {

        }

        return *this;
    }

} // marsupial::vulkan
