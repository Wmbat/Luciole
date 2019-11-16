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

#include <luciole/luciole.hpp>
#include <luciole/vk/shaders/shader_compiler.hpp>

int main( )
{
   ui::window::create_info const create_info 
   {
      .title = "Simple Triangle Example",
      .position = { 100, 100 },
      .size = { 1080, 720 }
   };

   auto wnd = ui::window( ui::window::create_info_t( create_info ) );    
   auto ctx = context( wnd );
   auto rdr = renderer( p_context_t( &ctx ), wnd );

   vk::shader_compiler* p_shader_compiler = new vk::shader_compiler( );

   auto vert_shader_id = rdr.load_shader( p_shader_compiler, vk::shader::filepath_t( "../data/shaders/default_shader.vert" ) );
   auto frag_shader_id = rdr.load_shader( p_shader_compiler, vk::shader::filepath_t( "../data/shaders/default_shader.frag" ) ); 

   while( wnd.is_open() )
   {
      rdr.draw_frame();

      wnd.poll_events();
   }

   delete p_shader_compiler;
   p_shader_compiler = nullptr;

   return 0;
}
