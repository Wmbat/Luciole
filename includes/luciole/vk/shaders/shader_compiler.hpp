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

#include <luciole/vk/shaders/shader_loader_interface.hpp>

namespace vk::shader
{
   class compiler : public loader_interface
   {
   public:
      compiler( ) = default;
      virtual ~compiler( ) = default;

      virtual shader_data load_shader( shader::filepath_view_t filepath ) const override;

   private:
      std::string_view get_filepath( std::string_view str ) const;
      std::string_view get_suffix( std::string_view name ) const;

      EShLanguage get_shader_stage( std::string_view stage ) const;
      type get_shader_type( EShLanguage shader_stage ) const;
   }; // class shader_compiler
} // namespace vk::shader
