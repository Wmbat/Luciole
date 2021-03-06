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

#include <luciole/vk/errors.hpp>

namespace vk
{
   error::error( result_t result ) noexcept
   {
      err_code = to_type( result.value( ) );
   }

   error::error( type_t type ) noexcept
   {
      err_code = type.value( );
   }

   std::string const& error::to_string() const
   {
      return string_type[static_cast<std::size_t>( err_code )];
   }

   bool error::is_error( ) const noexcept
   {
      return err_code != type::e_none;
   }  

   error::type error::get_type( ) const noexcept
   {
      return err_code;
   }
} // namespace vk
