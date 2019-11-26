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
#include <luciole/vk/shaders/shader_compiler.hpp>

namespace vk::shader
{
   const TBuiltInResource default_built_in_resource = {
      /* .MaxLights = */ 32,
      /* .MaxClipPlanes = */ 6,
      /* .MaxTextureUnits = */ 32,
      /* .MaxTextureCoords = */ 32,
      /* .MaxVertexAttribs = */ 64,
      /* .MaxVertexUniformComponents = */ 4096,
      /* .MaxVaryingFloats = */ 64,
      /* .MaxVertexTextureImageUnits = */ 32,
      /* .MaxCombinedTextureImageUnits = */ 80,
      /* .MaxTextureImageUnits = */ 32,
      /* .MaxFragmentUniformComponents = */ 4096,
      /* .MaxDrawBuffers = */ 32,
      /* .MaxVertexUniformVectors = */ 128,
      /* .MaxVaryingVectors = */ 8,
      /* .MaxFragmentUniformVectors = */ 16,
      /* .MaxVertexOutputVectors = */ 16,
      /* .MaxFragmentInputVectors = */ 15,
      /* .MinProgramTexelOffset = */ -8,
      /* .MaxProgramTexelOffset = */ 7,
      /* .MaxClipDistances = */ 8,
      /* .MaxComputeWorkGroupCountX = */ 65535,
      /* .MaxComputeWorkGroupCountY = */ 65535,
      /* .MaxComputeWorkGroupCountZ = */ 65535,
      /* .MaxComputeWorkGroupSizeX = */ 1024,
      /* .MaxComputeWorkGroupSizeY = */ 1024,
      /* .MaxComputeWorkGroupSizeZ = */ 64,
      /* .MaxComputeUniformComponents = */ 1024,
      /* .MaxComputeTextureImageUnits = */ 16,
      /* .MaxComputeImageUniforms = */ 8,
      /* .MaxComputeAtomicCounters = */ 8,
      /* .MaxComputeAtomicCounterBuffers = */ 1,
      /* .MaxVaryingComponents = */ 60,
      /* .MaxVertexOutputComponents = */ 64,
      /* .MaxGeometryInputComponents = */ 64,
      /* .MaxGeometryOutputComponents = */ 128,
      /* .MaxFragmentInputComponents = */ 128,
      /* .MaxImageUnits = */ 8,
      /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
      /* .MaxCombinedShaderOutputResources = */ 8,
      /* .MaxImageSamples = */ 0,
      /* .MaxVertexImageUniforms = */ 0,
      /* .MaxTessControlImageUniforms = */ 0,
      /* .MaxTessEvaluationImageUniforms = */ 0,
      /* .MaxGeometryImageUniforms = */ 0,
      /* .MaxFragmentImageUniforms = */ 8,
      /* .MaxCombinedImageUniforms = */ 8,
      /* .MaxGeometryTextureImageUnits = */ 16,
      /* .MaxGeometryOutputVertices = */ 256,
      /* .MaxGeometryTotalOutputComponents = */ 1024,
      /* .MaxGeometryUniformComponents = */ 1024,
      /* .MaxGeometryVaryingComponents = */ 64,
      /* .MaxTessControlInputComponents = */ 128,
      /* .MaxTessControlOutputComponents = */ 128,
      /* .MaxTessControlTextureImageUnits = */ 16,
      /* .MaxTessControlUniformComponents = */ 1024,
      /* .MaxTessControlTotalOutputComponents = */ 4096,
      /* .MaxTessEvaluationInputComponents = */ 128,
      /* .MaxTessEvaluationOutputComponents = */ 128,
      /* .MaxTessEvaluationTextureImageUnits = */ 16,
      /* .MaxTessEvaluationUniformComponents = */ 1024,
      /* .MaxTessPatchComponents = */ 120,
      /* .MaxPatchVertices = */ 32,
      /* .MaxTessGenLevel = */ 64,
      /* .MaxViewports = */ 16,
      /* .MaxVertexAtomicCounters = */ 0,
      /* .MaxTessControlAtomicCounters = */ 0,
      /* .MaxTessEvaluationAtomicCounters = */ 0,
      /* .MaxGeometryAtomicCounters = */ 0,
      /* .MaxFragmentAtomicCounters = */ 8,
      /* .MaxCombinedAtomicCounters = */ 8,
      /* .MaxAtomicCounterBindings = */ 1,
      /* .MaxVertexAtomicCounterBuffers = */ 0,
      /* .MaxTessControlAtomicCounterBuffers = */ 0,
      /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
      /* .MaxGeometryAtomicCounterBuffers = */ 0,
      /* .MaxFragmentAtomicCounterBuffers = */ 1,
      /* .MaxCombinedAtomicCounterBuffers = */ 1,
      /* .MaxAtomicCounterBufferSize = */ 16384,
      /* .MaxTransformFeedbackBuffers = */ 4,
      /* .MaxTransformFeedbackInterleavedComponents = */ 64,
      /* .MaxCullDistances = */ 8,
      /* .MaxCombinedClipAndCullDistances = */ 8,
      /* .MaxSamples = */ 4,
      /* maxMeshOutputVerticesNV = */ 1024,
      /* maxMeshOutputPrimitivesNV = */ 1024,
      /* maxMeshWorkGroupSizeX_NV =*/1024,
      /* maxMeshWorkGroupSizeY_NV = */ 1024,
      /* maxMeshWorkGroupSizeZ_NV = */ 1024,
      /* maxTaskWorkGroupSizeX_NV = */ 1024,
      /* maxTaskWorkGroupSizeY_NV = */ 1024,
      /* maxTaskWorkGroupSizeZ_NV = */ 1024,
      /* maxMeshViewCountNV = */ 1024,
      /* .limits = */
      {
         /* .nonInductiveForLoops = */ true,
         /* .whileLoops = */ true,
         /* .doWhileLoops = */ true,
         /* .generalUniformIndexing = */ true,
         /* .generalAttributeMatrixVectorIndexing = */ true,
         /* .generalVaryingIndexing = */ true,
         /* .generalSamplerIndexing = */ true,
         /* .generalVariableIndexing = */ true,
         /* .generalConstantMatrixVectorIndexing = */ true,
      }};

   compiler::shader_data compiler::load_shader( shader::filepath_view_t filepath ) const
   {
      std::string data = read_from_file( filepath.value( ) );
      char const* raw_data = data.c_str( );

      auto const shader_stage = get_shader_stage( get_suffix( filepath.value( ) ) );

      int client_input_semantics_version = 100;
      glslang::EShTargetClientVersion vulkan_client_version = glslang::EShTargetVulkan_1_1;
      glslang::EShTargetLanguageVersion target_version = glslang::EShTargetSpv_1_3;

      glslang::TShader glsl_shader( shader_stage );
      glsl_shader.setStrings( &raw_data, 1 );
      glsl_shader.setEnvInput( glslang::EShSourceGlsl, shader_stage, glslang::EShClientVulkan, client_input_semantics_version );
      glsl_shader.setEnvClient( glslang::EShClientVulkan, vulkan_client_version );
      glsl_shader.setEnvTarget( glslang::EShTargetSpv, target_version );

      TBuiltInResource resources = default_built_in_resource;

      EShMessages messages = ( EShMessages )( EShMsgSpvRules | EShMsgVulkanRules );

      int const default_version = 100;

      DirStackFileIncluder includer;
      includer.pushExternalLocalDirectory( get_filepath( filepath.value( ) ).data( ) );

      std::string preprocessed_glsl;

      if ( !glsl_shader.preprocess( &resources, default_version, ENoProfile, false, false, messages, &preprocessed_glsl, includer ) )
      {
         // handle error.
      }

      char const* raw_preprocessed_glsl = preprocessed_glsl.c_str( );
      glsl_shader.setStrings( &raw_preprocessed_glsl, 1 );

      if ( !glsl_shader.parse( &resources, default_version, false, messages ) )
      {
         // handle error
      }

      glslang::TProgram program;
      program.addShader( &glsl_shader );

      if ( !program.link( messages ) )
      {
         // handle error
      }

      std::vector<std::uint32_t> spir_v;

      spv::SpvBuildLogger logger;
      glslang::SpvOptions spv_options;

      glslang::GlslangToSpv( *program.getIntermediate( shader_stage ), spir_v, &logger, &spv_options );

      auto test = logger.getAllMessages( );

      return std::pair{spir_v, get_shader_type( shader_stage )};
   }

   std::string_view compiler::get_filepath( std::string_view str ) const { return str.substr( 0, str.find_last_of( "/\\" ) ); }

   std::string_view compiler::get_suffix( std::string_view name ) const
   {
      std::size_t const pos = name.rfind( '.' );

      return pos == std::string::npos ? "" : name.substr( name.rfind( '.' ) + 1 );
   }

   EShLanguage compiler::get_shader_stage( std::string_view stage ) const
   {
      if ( stage == "vert" )
      {
         return EShLangVertex;
      }
      else if ( stage == "tesc" )
      {
         return EShLangTessControl;
      }
      else if ( stage == "tese" )
      {
         return EShLangTessEvaluation;
      }
      else if ( stage == "geom" )
      {
         return EShLangGeometry;
      }
      else if ( stage == "frag" )
      {
         return EShLangFragment;
      }
      else if ( stage == "comp" )
      {
         return EShLangCompute;
      }
      else
      {
         assert( 0 && "Unknown shader stage" );
         return EShLangCount;
      }
   }

   shader::type compiler::get_shader_type( EShLanguage shader_stage ) const
   {
      switch ( shader_stage )
      {
         case EShLangVertex:
            return shader::type::e_vertex;
         case EShLangTessControl:
            return shader::type::e_tess_control;
         case EShLangTessEvaluation:
            return shader::type::e_tess_eval;
         case EShLangGeometry:
            return shader::type::e_geometry;
         case EShLangFragment:
            return shader::type::e_fragment;
         case EShLangCompute:
            return shader::type::e_compute;
         default:
            assert( 0 && "Unknown shader stage" );
            return shader::type::e_count;
      }
   }
} // namespace vk::shader
