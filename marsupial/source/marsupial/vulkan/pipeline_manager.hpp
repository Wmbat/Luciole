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

#ifndef MARSUPIAL_VULKAN_PIPELINE_MANAGER_HPP
#define MARSUPIAL_VULKAN_PIPELINE_MANAGER_HPP

#include <unordered_map>

#include "pipeline.hpp"
#include "../marsupial_core.hpp"

namespace marsupial::vulkan
{
    class pipeline_manager
    {
    public:
        MARSUPIAL_API pipeline_manager( ) = default;
        MARSUPIAL_API pipeline_manager( const pipeline_manager& rhs ) = delete;
        MARSUPIAL_API pipeline_manager( pipeline_manager&& rhs ) noexcept;
        MARSUPIAL_API ~pipeline_manager( ) = default;
        
        MARSUPIAL_API pipeline_manager& operator=( const pipeline_manager& rhs ) = delete;
        MARSUPIAL_API pipeline_manager& operator=( pipeline_manager&& rhs ) noexcept;

        template<pipeline_type T>
        std::enable_if_t<T == pipeline_type::graphics, uint32_t> insert( const graphics_pipeline::create_info& create_info )
        {
            graphics_pipelines_.emplace( std::pair{ ++pipeline_id_count_, graphics_pipeline{ create_info } } );
        
            return pipeline_id_count_;
        }
        
        /*
        template<pipeline_type T>
        std::enable_if_t<T == pipeline_type::compute, uint32_t> insert( const pipeline_create_info& create_info )
        {
            compute_pipelines_.emplace( std::pair{ ++pipeline_id_count_, compute_pipeline{ create_info } } );
            
            return pipeline_id_count_;
        }
        */

        template<pipeline_type T>
        std::enable_if_t<T == pipeline_type::graphics, const pipeline<T>&> find( const uint32_t id ) const
        {
            const auto it = graphics_pipelines_.find( id );
            
            assert( it != graphics_pipelines_.cend() );
            
            return it->second;
        }
        
        template<pipeline_type T>
        std::enable_if_t<T == pipeline_type::compute, const pipeline<T>&> find( const uint32_t id ) const
        {
            const auto it = compute_pipelines_.find( id );
            
            assert( it != compute_pipelines_.cend() );
            
            return it->second;
        }
        
    private:
        std::unordered_map<uint32_t, graphics_pipeline> graphics_pipelines_;
        std::unordered_map<uint32_t, compute_pipeline> compute_pipelines_;
        
        static inline uint32_t pipeline_id_count_;
    };
}

#endif //MARSUPIAL_VULKAN_PIPELINE_MANAGER_HPP
