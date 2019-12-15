#version 450
#extension GL_GOOGLE_include_directive : enable

layout (binding = 0) uniform uniform_buffer_object
{
   mat4 model;
   mat4 view;
   mat4 proj;
} ubo;

layout( location = 0 ) in vec2 in_position;
layout( location = 1 ) in vec3 in_colour;

layout( location = 0 ) out vec3 frag_colour;

void main( )
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4( in_position, 0.0, 1.0 );
    frag_colour = in_colour;
}
