#version 410
// ins (inputs)
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
// out (outputs)
out vec2 uv;

void main()
{
    vec4 positionH=vec4(vertexPosition,1);
    gl_Position =  P*V*M*positionH;
    uv=vertexUV;
}
