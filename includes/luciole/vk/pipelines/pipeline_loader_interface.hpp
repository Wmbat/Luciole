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

#ifndef LUCIOLE_VK_PIPELINES_PIPELINE_LOADER_INTERFACE_HPP
#define LUCIOLE_VK_PIPELINES_PIPELINE_LOADER_INTERFACE_HPP

#include <luciole/utils/strong_types.hpp>
#include <luciole/vk/pipelines/pipeline.hpp>
#include <luciole/vk/pipelines/pipeline_data.hpp>

namespace vk::pipeline
{
   class loader_interface
   { 
   public:
      loader_interface( ) = default;
      virtual ~loader_interface( ) = default;

      virtual data load_pipeline( filepath_view_t const& filepath ) const = 0;
   };

   using loader_ptr_t = strong_type<loader_interface const*, loader_interface>;
} // namespace vk

#endif // LUCIOLE_VK_PIPELINES_PIPELINE_LOADER_INTERFACE_HPP
