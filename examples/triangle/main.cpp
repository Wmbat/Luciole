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

#include <luciole/core/context.hpp>
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

   auto wnd = ui::window( window_create_info );
   auto context = core::context( wnd, &main_logger );

   return 0;
}
