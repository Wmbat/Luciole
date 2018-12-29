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

#include "vk_utils.h"
#include "graphics/shader.h"

namespace TWE
{
    class pipeline_cache
    {
    public:
        using id = std::uint32_t;
        
    public:
        pipeline_cache( const vk::Device& device );
        
    private:
        vk::UniquePipelineCache cache_;
    };
    
    class pipeline_layout
    {
    public:
        using id = std::uint32_t;
        
    public:
        pipeline_layout( const vk::Device& device );
        pipeline_layout( const pipeline_layout& rhs ) = delete;
        pipeline_layout( pipeline_layout&& rhs ) noexcept;
        ~pipeline_layout( ) = default;
        
        pipeline_layout& operator=( const pipeline_layout& rhs ) = delete;
        pipeline_layout& operator=( pipeline_layout&& rhs ) noexcept;
        
        const vk::PipelineLayout& get( ) const noexcept;
    
    private:
        vk::UniquePipelineLayout layout_;
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
        pipeline( vk::Device* p_device, vk::Pipeline&& pipeline, const type type,
            const pipeline_layout::id layout_id );
        pipeline( const pipeline& rhs ) = delete;
        pipeline( pipeline&& rhs ) noexcept;
        ~pipeline( );
        
        pipeline& operator=( const pipeline& rhs ) = delete;
        pipeline& operator=( pipeline&& rhs ) noexcept;
        
        vk::PipelineBindPoint get_bind_point( ) const;
        const vk::Pipeline& get( ) const;
        
    private:
        vk::Device* p_device_;
        
        vk::Pipeline pipeline_;
        type type_;
        
        pipeline_layout::id layout_id_;
        pipeline_cache::id cache_id_;
    };
}

#endif //TWE_VK_PIPELINE_H
