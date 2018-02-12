#version 410
// ins (inputs)
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
uniform mat4 MVP;
// out (outputs)
out vec2 uv;

void main()
{
    vec4 positionH=vec4(vertexPosition,1);
    gl_Position =  MVP*positionH;
    uv=vertexUV;
}
