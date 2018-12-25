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

#ifndef TWE_VK_PIPELINE_MANAGER_H
#define TWE_VK_PIPELINE_MANAGER_H

#include <unordered_map>

#include "vk_utils.h"
#include "vk_shader_manager.h"

namespace TWE
{
    class vk_pipeline_manager
    {
    private:
        struct pipeline_type
        {
            vk::Pipeline pipeline_;
            vk::PipelineLayout layout_;
        };
        
    public:
        struct pipeline_create_info
        {
            std::uint32_t vert_shader_id_ = 0;
            std::uint32_t frag_shader_id_ = 0;
            vk::Extent2D swapchain_extent_;
        };
        
    public:
        std::uint32_t create_pipeline( const pipeline_create_info& create_info );
        
    private:
        vk::Device* p_device_;
        
        vk_shader_manager* p_shader_manager_;
        
        std::unordered_map<std::uint32_t, pipeline_type> pipelines_;
        
        static std::uint32_t pipeline_id_count_;
    };
}

#endif //TWE_VK_PIPELINE_MANAGER_H
