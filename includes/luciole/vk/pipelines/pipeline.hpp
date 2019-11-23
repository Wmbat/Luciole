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

#ifndef LUCIOLE_VK_PIPELINES_PIPELINE_HPP
#define LUCIOLE_VK_PIPELINES_PIPELINE_HPP

#include <luciole/context.hpp>
#include <luciole/utils/strong_types.hpp>
#include <luciole/vk/shaders/shader_manager.hpp>

#include <string>
#include <string_view>

namespace vk::pipeline
{
   class unique_pipeline
   {
   public: 
      enum class type
      {
         e_graphics = 0,
         e_computer = 1,
         e_count = 2
      };

      struct create_info
      {
         context const* p_context = nullptr;
         shader::manager const* p_shader_manager = nullptr;

         type pipeline_type = type::e_count;
         shader::set::id shader_set_id; 
      };

      using create_info_t = strong_type<create_info const&, unique_pipeline>;

   public:
      unique_pipeline( ); 
      unique_pipeline( create_info_t const& create_info );
      unique_pipeline( unique_pipeline const& rhs ) = delete;
      unique_pipeline( unique_pipeline&& rhs );
      ~unique_pipeline( );

      unique_pipeline& operator=( unique_pipeline const& rhs ) = delete;
      unique_pipeline& operator=( unique_pipeline&& rhs );

   private:
      context const* p_context;

      type pipeline_type;

      VkPipeline handle;
   }; // class unique_pipeline

   using id = std::uint32_t;
   using id_t = strong_type<id, unique_pipeline>;
   using filepath_t = strong_type<std::string const&, unique_pipeline>; 
   using filepath_view_t = strong_type<std::string_view, unique_pipeline>;
} // namespace vk::pipeline

#endif // LUCIOLE_VK_PIPELINES_PIPELINE_HPP
