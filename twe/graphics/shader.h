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

#include "../vk_utils.h"

namespace twe
{
    /*!
     * @brief
     */
    class shader
    {
    public:
        using id = std::uint32_t;
        
        /*!
         * @brief
         */
        enum class type
        {
            vertex,
            fragment,
            compute,
            tesselation,
            geometry
        };
        
        /*!
         * @brief
         */
        struct create_info
        {
            vk::Device& device_;
            
            type type_;
            
            std::string filepath_;
            std::string entry_point_;
            
            create_info& set_device( const vk::Device& device )
            {
                device_ = device;
                
                return *this;
            }
            create_info& set_stage( const type& type )
            {
                type_ = type;
                
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
        shader( const create_info& create_info );
        shader( const shader& rhs ) noexcept = delete;
        shader( shader&& rhs ) noexcept;
        ~shader( ) = default;
        
        shader& operator=( const shader& rhs ) noexcept = delete;
        shader& operator=( shader&& rhs ) noexcept;
        
        const vk::PipelineShaderStageCreateInfo get_shader_stage_create_info( ) const noexcept;
    
    private:
        vk::UniqueShaderModule shader_;
        type type_;
    
        std::string shader_filepath_;
        std::string entry_point_;
    };
}

#endif //TWE_VK_SHADER_H
