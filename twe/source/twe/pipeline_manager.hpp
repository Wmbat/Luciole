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

#ifndef ENGINE_PIPELINE_MANAGER_HPP
#define ENGINE_PIPELINE_MANAGER_HPP

#include <unordered_map>

#include "twe_core.hpp"
#include "pipeline.hpp"

namespace twe
{
    class pipeline_manager
    {
    public:
        TWE_API pipeline_manager( ) = default;
        TWE_API pipeline_manager( const pipeline_manager& rhs ) = delete;
        TWE_API pipeline_manager( pipeline_manager&& rhs ) noexcept;
        TWE_API ~pipeline_manager( ) = default;
        
        TWE_API pipeline_manager& operator=( const pipeline_manager& rhs ) = delete;
        TWE_API pipeline_manager& operator=( pipeline_manager&& rhs ) noexcept;
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, graphics_pipeline>, uint32_t> insert( const pipeline_create_info& create_info )
        {
            graphics_pipelines_.emplace( std::pair{ ++pipeline_id_count_, graphics_pipeline{ create_info } } );
        
            return pipeline_id_count_;
        }
        
        template<class C>
        std::enable_if_t<std::is_same_v<C, compute_pipeline>, uint32_t> insert( const pipeline_create_info& create_info )
        {
            compute_pipelines_.emplace( std::pair{ ++pipeline_id_count_, compute_pipeline{ create_info } } );
            
            return pipeline_id_count_;
        }
        
        template<class C, class = std::enable_if_t<std::is_same_v<C, graphics_pipeline>>>
        const graphics_pipeline& find( const uint32_t id ) const
        {
            const auto it = graphics_pipelines_.find( id );
            
            assert( it != graphics_pipelines_.cend() );
            
            return it->second;
        }
        
        template<class C, class = std::enable_if_t<std::is_same_v<C, compute_pipeline>>>
        const compute_pipeline& find( const uint32_t id ) const
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

#endif //ENGINE_PIPELINE_MANAGER_HPP