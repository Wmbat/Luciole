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

#ifndef TWE_VK_SHADER_H
#define TWE_VK_SHADER_H

#include "vk_utils.h"

namespace TWE
{
    class vk_shader
    {
    public:
        struct create_info
        {
            vk::Device* p_device_;
            
            vk::ShaderStageFlagBits shader_stage_;
            
            std::string filepath_;
            std::string entry_point_;
            
            create_info& set_p_device( vk::Device* p_device )
            {
                p_device_ = p_device;
                
                return *this;
            }
            create_info& set_stage( const vk::ShaderStageFlagBits& stage )
            {
                shader_stage_ = stage;
                
                return *this;
            }
            create_info& set_filepath( const std::string& filepath )
            {
                filepath_ = filepath;
                
                return *this;
            }
            create_info& set_entry_point( const std::string& entry_point )
            {
                entry_point_ = entry_point;
                
                return *this;
            }
        };
        
    public:
        vk_shader( const create_info& create_info );
        vk_shader( const vk_shader& rhs ) noexcept = delete;
        vk_shader( vk_shader&& rhs ) noexcept;
        ~vk_shader( );
        
        vk_shader& operator=( const vk_shader& rhs ) noexcept = delete;
        vk_shader& operator=( vk_shader&& rhs ) noexcept;
        
        const vk::PipelineShaderStageCreateInfo get_shader_stage_create_info( ) const noexcept;
    
    private:
        vk::Device* p_device_;
        
        vk::ShaderModule shader_;
        vk::ShaderStageFlagBits shader_stage_;
        
        std::string entry_point_;
    };
}

#endif //TWE_VK_SHADER_H
