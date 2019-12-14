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

#include <variant>

namespace vk::pipeline
{
   class loader_interface
   {
   public:
      enum class error_code
      {
         e_incorrect_filepath = 0,
         e_empty_filepath = 1,
         e_no_pipeline_data = 2,
         e_no_pipeline_type = 3
      };

      using load_result = std::variant<data, loader_interface::error_code>;

   protected:
      loader_interface( ) = default;
      virtual ~loader_interface( ) = default;

   public:
      virtual load_result load_pipeline( filepath_view_t const& filepath ) const = 0;
   }; // class loader_interface

   using loader_ptr_t = strong_type<loader_interface const*, loader_interface>;
} // namespace vk::pipeline

#endif // LUCIOLE_VK_PIPELINES_PIPELINE_LOADER_INTERFACE_HPP
