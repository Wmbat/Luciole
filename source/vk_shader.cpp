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

#include <vk_shader.h>

#include "vk_shader.h"
#include "log.h"

#include "utilities/file_io.h"

namespace TWE
{
    vk_shader::vk_shader( const vk_shader::create_info& create_info )
        :
        p_device_( create_info.p_device_ ),
        type_( create_info.type_ ),
        entry_point_( create_info.entry_point_ )
    {
        const std::string shader_code = read_from_binary_file( create_info.filepath_ );
    
        const auto module_create_info = vk::ShaderModuleCreateInfo( )
            .setCodeSize( shader_code.size( ) )
            .setPCode( reinterpret_cast<const uint32_t*>( shader_code.data() ) );
        
        try
        {
            auto res = p_device_->createShaderModuleUnique( module_create_info );
            check_vk_result( res.result, "vk_shader ctor -> Failed to create Shader Module: " + create_info.filepath_ );
            
            shader_.swap( res.value );
        }
        catch( const vk_error& e )
        {
            core_error( e.what() );
        }
    }
    
    vk_shader::vk_shader( vk_shader&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    
    vk_shader& vk_shader::operator=( vk_shader&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            p_device_ = rhs.p_device_;
            rhs.p_device_ = nullptr;
            
            shader_ = std::move( rhs.shader_ );
            
            type_ = rhs.type_;
            rhs.type_ = type{ };
            
            entry_point_ = std::move( rhs.entry_point_ );
        }
        
        return *this;
    }
    
    const vk::PipelineShaderStageCreateInfo vk_shader::get_shader_stage_create_info( ) const noexcept
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


