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

#include "shader.hpp"
#include "../utilities/log.hpp"
#include "../utilities/vk_error.hpp"
#include "../utilities/file_io.hpp"

namespace twe
{
    shader::shader( const shader::create_info& create_info )
        :
        type_( create_info.type_ ),
        entry_point_( create_info.entry_point_ )
    {
        const std::string shader_code = read_from_binary_file( create_info.filepath_ );
    
        const auto module_create_info = vk::ShaderModuleCreateInfo( )
            .setCodeSize( shader_code.size( ) )
            .setPCode( reinterpret_cast<const uint32_t*>( shader_code.data() ) );
        
        try
        {
            shader_ = create_info.device_.createShaderModuleUnique( module_create_info );
        }
        catch( const vk_error& e )
        {
            // TODO: properly handle error.
            core_error( e.what() );
        }
    }
    
    shader::shader( shader&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    
    shader& shader::operator=( shader&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            shader_ = std::move( rhs.shader_ );
            
            type_ = rhs.type_;
            rhs.type_ = type{ };
            
            entry_point_ = std::move( rhs.entry_point_ );
        }
        
        return *this;
    }
    
    const vk::PipelineShaderStageCreateInfo shader::get_shader_stage_create_info( ) const noexcept
    {
        vk::ShaderStageFlagBits stage_flag{ };
        
        if( type_ == type::vertex )
        {
            stage_flag = vk::ShaderStageFlagBits::eVertex;
        }
        else if( type_ == type::fragment )
        {
            stage_flag = vk::ShaderStageFlagBits::eFragment;
        }
        else if( type_ == type::compute )
        {
            stage_flag = vk::ShaderStageFlagBits::eCompute;
        }
        else if( type_ == type::geometry )
        {
            stage_flag = vk::ShaderStageFlagBits::eGeometry;
        }
        
        return vk::PipelineShaderStageCreateInfo( )
                    .setModule( shader_.get() )
                    .setStage( stage_flag )
                    .setPName( entry_point_.c_str( ) );
    }
}


