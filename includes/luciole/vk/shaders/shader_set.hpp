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

#ifndef LUCIOLE_VK_SHADERS_SHADER_PACK_HPP
#define LUCIOLE_VK_SHADERS_SHADER_PACK_HPP

#include <luciole/luciole_core.hpp>
#include <luciole/utils/strong_types.hpp>
#include <luciole/vk/shaders/shader.hpp>

#include <vulkan/vulkan.h>

#include <optional>
#include <cstdint>
#include <vector>

namespace vk::shader
{
   class set
   {
   public:
      using id = std::uint32_t;
      using id_t = strong_type<id, set>;

      struct create_info
      {  
         std::optional<id> vertex_id = std::nullopt;
         std::optional<id> tess_control_id = std::nullopt;
         std::optional<id> tess_eval_id = std::nullopt;
         std::optional<id> geometry_id = std::nullopt;
         std::optional<id> fragment_id = std::nullopt;
         std::optional<id> compute_id = std::nullopt;
      };

      using create_info_t = strong_type<create_info const&, set>;
      
   public: 
      set( );
      set( create_info_t const& create_info );
      
      std::optional<id> get_vertex_shader( ) const PURE;
      std::optional<id> get_tess_control_shader( ) const PURE;
      std::optional<id> get_tess_evaluation_shader( ) const PURE;
      std::optional<id> get_geometry_shader( ) const PURE;
      std::optional<id> get_fragment_shader( ) const PURE;
      std::optional<id> get_compute_shader( ) const PURE;
     
   private:
      std::optional<id> vertex_id;
      std::optional<id> tess_control_id;
      std::optional<id> tess_eval_id;
      std::optional<id> geometry_id;
      std::optional<id> fragment_id;
      std::optional<id> compute_id; 
   };
} // namespace vk

#endif // LUCIOLE_VK_SHADERS_SHADER_PACK_HPP
