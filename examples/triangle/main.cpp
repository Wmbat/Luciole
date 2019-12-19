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
#include <luciole/utils/logger.hpp>
#include <luciole/vk/pipelines/pipeline_json_loader.hpp>
#include <luciole/vk/shaders/shader_compiler.hpp>

int main( )
{
   logger main_logger( "engine logger" );

   auto window_create_info = ui::window::create_info{};
   window_create_info.title = "Simple Triangle Example";
   window_create_info.position = {100, 100};
   window_create_info.size = {1080, 720};
   window_create_info.p_logger = &main_logger;

   auto wnd = ui::window( ui::window::create_info_t( window_create_info ) );
   auto ctx = context( wnd, &main_logger );
   auto rdr = renderer( &ctx, wnd );

   auto const p_shader_compiler = std::make_unique<vk::shader::compiler>( );
   auto const p_pipeline_loader = std::make_unique<vk::pipeline::json_loader>( );

   auto pack_create_info = vk::shader::set::create_info{};

   pack_create_info.vertex_id = rdr.load_shader_module(
      vk::shader::loader_ptr_t( p_shader_compiler.get( ) ), vk::shader::filepath_t( "../data/shaders/default_shader.vert" ) );

   pack_create_info.fragment_id = rdr.load_shader_module(
      vk::shader::loader_ptr_t( p_shader_compiler.get( ) ), vk::shader::filepath_t( "../data/shaders/default_shader.frag" ) );

   auto const shader_pack_id = rdr.create_shader_pack( vk::shader::set::create_info_t( pack_create_info ) );

   rdr.create_pipeline( vk::pipeline::loader_ptr_t( p_pipeline_loader.get( ) ), vk::shader::set::id_t( shader_pack_id ),
      vk::pipeline::filepath_view_t( "../data/pipelines/default_graphics_pipeline.json" ) );

   /*
      while( wnd.is_open() )
      {
         rdr.draw_frame();

         wnd.poll_events();
      }
   */
   return 0;
}
