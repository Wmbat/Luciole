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

#include <luciole/utils/file_io.hpp>
#include <luciole/vk/pipelines/pipeline_json_loader.hpp>

#include <nlohmann/json.hpp>

#include <string>

namespace vk::pipeline
{
   loader_interface::load_result json_loader::load_pipeline( filepath_view_t const& filepath ) const
   {
      if ( filepath.value( ).empty( ) )
      {
         return loader_interface::error_code::e_empty_filepath;
      }

      data json_data = data{};

      nlohmann::json const pipeline_json = nlohmann::json::parse( read_from_file( filepath.value( ) ) );

      if ( pipeline_json.contains( pipeline_id ) )
      {
         auto const pipeline = pipeline_json[pipeline_id];

         if ( pipeline.contains( pipeline_type_id ) )
         {
            std::string const type = pipeline[pipeline_type_id];
         }
         else
         {
            return loader_interface::error_code::e_no_pipeline_type;
         }

         if ( pipeline.contains( input_assembly_id ) )
         {
            auto const input_assembly = pipeline[input_assembly_id];

            auto create_info = VkPipelineInputAssemblyStateCreateInfo{};
            create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            create_info.pNext = nullptr;
            create_info.flags = 0;

            if ( input_assembly.contains( topology_id ) )
            {
               auto const topology = input_assembly.at( topology_id );
               if ( topology == "point_list" )
               {
                  create_info.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
               }
               else if ( topology == "line_list" )
               {
                  create_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
               }
               else if ( topology == "line_strip" )
               {
                  create_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
               }
               else if ( topology == "triangle_list" )
               {
                  create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
               }
               else if ( topology == "triangle_strip" )
               {
                  create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
               }
               else if ( topology == "triangle_fan" )
               {
                  create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
               }
               else if ( topology == "line_list_with_adjacency" )
               {
                  create_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
               }
               else if ( topology == "line_strip_with_adjacency" )
               {
                  create_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
               }
               else if ( topology == "triangle_list_with_adjacency" )
               {
                  create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
               }
               else if ( topology == "triangle_strip_with_ajacency" )
               {
                  create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
               }
               else if ( topology == "patch_list" )
               {
                  create_info.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
               }
               else
               {
                  create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
               }
            }
            else
            {
               create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            }

            if ( input_assembly.contains( primitive_restart_enable_id ) )
            {
               auto const restart_enable = input_assembly.at( primitive_restart_enable_id );
               if ( restart_enable.is_boolean( ) )
               {
                  create_info.primitiveRestartEnable = restart_enable;
               }
               else
               {
                  create_info.primitiveRestartEnable = false;
               }
            }
            else
            {
               create_info.primitiveRestartEnable = false;
            }
         }
         else
         {
         }
      }
      else
      {
         return loader_interface::error_code::e_no_pipeline_data;
      }
   }
} // namespace vk::pipeline
