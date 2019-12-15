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

#include <luciole/vk/shaders/shader_set.hpp>

namespace vk::shader
{
   set::set( ) :
      vertex_id( std::nullopt ), tess_eval_id( std::nullopt ), tess_control_id( std::nullopt ), geometry_id( std::nullopt ),
      fragment_id( std::nullopt ), compute_id( std::nullopt )
   {}

   set::set( create_info_t const& create_info ) :
      vertex_id( create_info.value( ).vertex_id ), tess_eval_id( create_info.value( ).tess_eval_id ),
      tess_control_id( create_info.value( ).tess_control_id ), geometry_id( create_info.value( ).geometry_id ),
      fragment_id( create_info.value( ).fragment_id ), compute_id( create_info.value( ).vertex_id )
   {}

   std::optional<id> set::get_vertex_shader( ) const { return vertex_id; }

   std::optional<id> set::get_tess_control_shader( ) const { return tess_control_id; }

   std::optional<id> set::get_tess_evaluation_shader( ) const { return tess_eval_id; }

   std::optional<id> set::get_geometry_shader( ) const { return geometry_id; }

   std::optional<id> set::get_fragment_shader( ) const { return fragment_id; }

   std::optional<id> set::get_compute_shader( ) const { return compute_id; }
} // namespace vk::shader

