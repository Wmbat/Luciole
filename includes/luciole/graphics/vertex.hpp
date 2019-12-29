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

#ifndef LUCIOLE_GRAPHICS_VERTEX_HPP
#define LUCIOLE_GRAPHICS_VERTEX_HPP

#include <luciole/vk/core.hpp>

#include <glm/glm.hpp>

#include <array>
#include <vector>

struct vertex
{
   glm::vec2 position;
   glm::vec3 colour;

   static VkVertexInputBindingDescription get_binding_description( )
   {
      VkVertexInputBindingDescription const description{.binding = 0, .stride = sizeof( vertex ), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};

      return description;
   }

   static std::array<VkVertexInputAttributeDescription, 2> get_attribute_descriptions( )
   {
      std::array<VkVertexInputAttributeDescription, 2> descriptions;
      descriptions[0].location = 0;
      descriptions[0].binding = 0;
      descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
      descriptions[0].offset = offsetof( vertex, position );
      descriptions[1].location = 1;
      descriptions[1].binding = 0;
      descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
      descriptions[1].offset = offsetof( vertex, colour );

      return descriptions;
   }
};

struct uniform_buffer_object
{
   glm::mat4 model;
   glm::mat4 view;
   glm::mat4 proj;
};

#endif // LUCIOLE_GRAPHICS_VERTEX_HPP
