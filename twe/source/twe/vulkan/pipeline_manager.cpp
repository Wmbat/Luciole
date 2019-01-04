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

#include "../utilities/log.hpp"
#include "pipeline_manager.hpp"

#include "../utilities/file_io.hpp"

namespace twe
{    
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
