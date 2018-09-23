#version 450
#extension GL_ARB_separate_shader_objects : enable

layout( location = 0 ) out vec4 frag_colour;

out gl_PerVertex
{
    vec4 gl_Position;
};

vec2 positions[3] = vec2[]
(
    vec2(  0.0f, -0.5f ),
    vec2(  0.5f,  0.5f ),
    vec2( -0.5f,  0.5f )
);

vec4 colours[3] = vec4[]
(
    vec4( 1.0f, 0.0f, 0.0f, 1.0f ),
    vec4( 0.0f, 1.0f, 0.0f, 1.0f ),
    vec4( 0.0f, 0.0f, 1.0f, 1.0f )
);

void main( )
{
    gl_Position = vec4( positions[gl_VertexIndex], 0.0, 1.0 );
    frag_colour = colours[gl_VertexIndex];
}