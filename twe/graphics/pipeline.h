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

#ifndef TWE_VK_PIPELINE_H
#define TWE_VK_PIPELINE_H

#include "shader.h"
#include "../vk_utils.h"

namespace twe
{
    class pipeline_cache
    {
    public:
        using id = std::uint32_t;
        
    public:
        TWE_API pipeline_cache( const vk::Device& device );
        
    private:
        vk::UniquePipelineCache cache_;
    };
    
    class pipeline_layout
    {
    public:
        using id = std::uint32_t;
        
    public:
        TWE_API pipeline_layout( const vk::Device& device, const shader::id vert_id, const shader::id frag_id );
        TWE_API pipeline_layout( const pipeline_layout& rhs ) = delete;
        TWE_API pipeline_layout( pipeline_layout&& rhs ) noexcept;
        TWE_API~pipeline_layout( ) = default;
        
        TWE_API pipeline_layout& operator=( const pipeline_layout& rhs ) = delete;
        TWE_API pipeline_layout& operator=( pipeline_layout&& rhs ) noexcept;
        
        TWE_API const vk::PipelineLayout& get( ) const noexcept;
    
    private:
        vk::UniquePipelineLayout layout_;
        
        shader::id vert_id_;
        shader::id frag_id_;
    };
    
    class pipeline
    {
    public:
        using id = std::uint32_t;
        
        enum class type
        {
            graphics,
            compute,
            ray_tracing
        };
        
    public:
        TWE_API pipeline( vk::UniquePipeline&& pipeline, const type type, const pipeline_layout::id layout_id );
        TWE_API pipeline( const pipeline& rhs ) = delete;
        TWE_API pipeline( pipeline&& rhs ) noexcept;
        TWE_API ~pipeline( ) = default;
        
        TWE_API pipeline& operator=( const pipeline& rhs ) = delete;
        TWE_API pipeline& operator=( pipeline&& rhs ) noexcept;
        
        TWE_API vk::PipelineBindPoint get_bind_point( ) const;
        TWE_API const vk::Pipeline& get( ) const;
        
    private:
        vk::UniquePipeline pipeline_;
        type type_;
        
        pipeline_layout::id layout_id_;
        pipeline_cache::id cache_id_;
    };
}

#endif //TWE_VK_PIPELINE_H
