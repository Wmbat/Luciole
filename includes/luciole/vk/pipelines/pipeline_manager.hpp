/**
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

#ifndef LUCIOLE_VK_PIPELINES_PIPELINE_MANAGER_HPP
#define LUCIOLE_VK_PIPELINES_PIPELINE_MANAGER_HPP

#include <luciole/context.hpp>
#include <luciole/vk/pipelines/pipeline.hpp>
#include <luciole/vk/pipelines/pipeline_loader_interface.hpp>
#include <luciole/vk/shaders/shader.hpp>

#include <unordered_map>

namespace vk::pipeline
{
   class manager
   {
   public:
      id create_pipeline( loader_ptr_t p_loader, shader::set::id_t pack_id, filepath_view_t filepath );

   private:
      std::unordered_map<id, unique_pipeline> pipelines;

      static inline id PIPELINE_ID_COUNT = 0;
   }; // class manager

} // namespace vk::pipeline

#endif // LUCIOLE_VK_PIPELINES_PIPELINE_MANAGER_HPP
