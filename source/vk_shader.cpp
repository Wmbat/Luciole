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
        shader_stage_( create_info.shader_stage_ ),
        entry_point_( create_info.entry_point_ )
    {
        const std::string shader_code = read_from_binary_file( create_info.filepath_ );
    
        const auto module_create_info = vk::ShaderModuleCreateInfo( )
            .setCodeSize( shader_code.size( ) )
            .setPCode( reinterpret_cast<const uint32_t*>( shader_code.data() ) );
        
        try
        {
            shader_ = check_vk_result_value(
                p_device_->createShaderModule( module_create_info ),
                "vk_shader ctor -> Failed to create Shader Module: " + create_info.filepath_ );
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
    
    vk_shader::~vk_shader( )
    {
        p_device_->destroyShaderModule( shader_ );
    }
    
    vk_shader& vk_shader::operator=( vk_shader&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            p_device_ = rhs.p_device_;
            rhs.p_device_ = nullptr;
            
            shader_ = rhs.shader_;
            rhs.shader_ = vk::ShaderModule( );
            
            shader_stage_ = rhs.shader_stage_;
            rhs.shader_stage_ = vk::ShaderStageFlagBits( );
            
            entry_point_ = std::move( rhs.entry_point_ );
        }
        
        return *this;
    }
    
    const vk::PipelineShaderStageCreateInfo vk_shader::get_shader_stage_create_info( ) const noexcept
    {
        return vk::PipelineShaderStageCreateInfo( )
                    .setModule( shader_ )
                    .setStage( shader_stage_ )
                    .setPName( entry_point_.c_str( ) );
    }
}


