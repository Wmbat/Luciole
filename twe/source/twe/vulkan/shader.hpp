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

#ifndef TWE_VK_SHADER_H
#define TWE_VK_SHADER_H

#include "core.hpp"
#include "../twe_core.hpp"
#include "../utilities/log.hpp"
#include "../utilities/file_io.hpp"

namespace twe::vulkan
{
    enum class shader_type
    {
        vertex,
        fragment,
        compute,
        geometry
    };
    
    struct shader_create_info
    {
        vk::Device device_;
        
        std::string filepath_;
        std::string entry_point_;
    
        shader_create_info& set_device( const vk::Device device )
        {
            device_ = device;
            return *this;
        }
        shader_create_info& set_filepath( const std::string& filepath )
        {
            filepath_ = filepath;
            return *this;
        }
        shader_create_info& set_entry_point( const std::string& entry_point )
        {
            entry_point_ = entry_point;
            return *this;
        }
    };
    
    template<shader_type T>
    class shader
    {
    public:
        shader( ) = default;
        shader( const shader_create_info& create_info )
            :
            entry_point_( create_info.entry_point_ )
        {
            const std::string shader_code = read_from_binary_file( create_info.filepath_ );
    
            const auto module_create_info = vk::ShaderModuleCreateInfo( )
                .setCodeSize( shader_code.size( ) )
                .setPCode( reinterpret_cast<const uint32_t*>( shader_code.data() ) );
    
            shader_ = create_info.device_.createShaderModuleUnique( module_create_info );
        }
        shader( const shader& rhs ) noexcept = delete;
        shader( shader&& rhs ) noexcept
        {
            *this = std::move( rhs );
        }
        ~shader( ) = default;
        
        shader& operator=( const shader& rhs ) noexcept = delete;
        shader& operator=( shader&& rhs ) noexcept
        {
            if( this != &rhs )
            {
                shader_ = std::move( rhs.shader_ );
        
                entry_point_ = std::move( rhs.entry_point_ );
            }
    
            return *this;
        }
        
        const vk::PipelineShaderStageCreateInfo get_shader_stage_create_info( ) const noexcept
        {
            vk::ShaderStageFlagBits stage_flag{ };
    
            if( T == shader_type::vertex )
            {
                stage_flag = vk::ShaderStageFlagBits::eVertex;
            }
            else if( T == shader_type::fragment )
            {
                stage_flag = vk::ShaderStageFlagBits::eFragment;
            }
            else if( T == shader_type::compute )
            {
                stage_flag = vk::ShaderStageFlagBits::eCompute;
            }
            else if( T == shader_type::geometry )
            {
                stage_flag = vk::ShaderStageFlagBits::eGeometry;
            }
    
            return vk::PipelineShaderStageCreateInfo( )
                .setModule( shader_.get() )
                .setStage( stage_flag )
                .setPName( entry_point_.c_str( ) );
        }
        
    private:
        vk::UniqueShaderModule shader_;
        
        std::string entry_point_;
    };
    
    using vertex_shader = shader<shader_type::vertex>;
    using fragment_shader = shader<shader_type::fragment>;
    using compute_shader = shader<shader_type::compute>;
}

#endif //TWE_VK_SHADER_H
