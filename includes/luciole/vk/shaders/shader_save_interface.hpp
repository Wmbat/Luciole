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

#ifndef LUCIOLE_VK_SHADER_SAVING_INTERFACE_HPP
#define LUCIOLE_VK_SHADER_SAVING_INTERFACE_HPP

#include <luciole/vk/shaders/shader.hpp>

#include <memory>

namespace vk::shader
{
   class save_interface
   {
   public:  
      virtual ~save_interface( );

      virtual void save_shader( unique_shader_t const& shader ) const = 0;
   };

   using save_ptr_t = strong_type<save_interface const*, save_interface>;
   using save_uptr_t = strong_type<std::unique_ptr<save_interface> &, save_interface>;
}

#endif // LUCIOLE_VK_SHADER_SAVING_INTERFACE_HPP
