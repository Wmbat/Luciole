#version 450

layout( location = 0 ) out vec4 frag_colour;

vec2 position[3] = vec2[](
    vec2( 0.0, -0.5 ),
    vec2( 0.5, 0.5 ),
    vec2( -0.5, 0.5 )
);

vec4 colours[3] = vec4[](
    vec4( 1.0, 0.0, 0.0, 1.0 ),
    vec4( 0.0, 1.0, 0.0, 1.0 ),
    vec4( 0.0, 0.0, 1.0, 1.0 )
);

void main( )
{
    gl_Position = vec4( position[gl_VertexIndex], 0.0, 1.0 );
    frag_colour = colours[gl_VertexIndex];
}