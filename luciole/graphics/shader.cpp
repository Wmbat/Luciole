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

#include "shader.hpp"

shader::shader( shader_filepath_t filepath )
{
    auto glsl_code = bzr::read_from_file( filepath.value_ ).c_str( );
    
    auto const stage = ( filepath.value_.rfind('.') == std::string::npos ) ? "" : filepath.value_.substr(filepath.value_.rfind('.') + 1);
    EShLanguage shader_type;
    
    if ( stage == "vert" )
    {
        shader_type = EShLangVertex;
    }
    else if ( stage == "frag" )
    {
        shader_type = EShLangFragment;
    }
    else
    {
        assert( 0 && "Unknow shader file" );
        abort( );
    }
    
    glslang::TShader shader( shader_type );
    shader.setStrings( &glsl_code, 1 );
    
    auto const vulkan_client_version = glslang::EShTargetVulkan_1_1;
    auto const target_version = glslang::EShTargetSpv_1_4;

    int client_input_semantics_version = 100;
    shader.setEnvInput( glslang::EShSourceGlsl, shader_type, glslang::EShClientVulkan, client_input_semantics_version );
    shader.setEnvClient( glslang::EShClientVulkan, vulkan_client_version );
    shader.setEnvTarget( glslang::EShTargetSpv, target_version );
}