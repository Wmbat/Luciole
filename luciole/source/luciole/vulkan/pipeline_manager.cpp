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

#include "pipeline_manager.hpp"

namespace lcl::vulkan
{
    pipeline_manager::pipeline_manager( pipeline_manager&& rhs ) noexcept
    {
        *this = std::move( rhs );
    }
    
    pipeline_manager& pipeline_manager::operator=( pipeline_manager&& rhs ) noexcept
    {
        if( this != &rhs )
        {
            graphics_pipelines_ = std::move( rhs.graphics_pipelines_ );
            
            pipeline_id_count_ = rhs.pipeline_id_count_;
            rhs.pipeline_id_count_ = 0;
        }
        
        return *this;
    }
}