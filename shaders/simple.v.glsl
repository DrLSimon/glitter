#version 410
// ins (inputs)
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;
// out (outputs)
out vec4 color;

uniform float time;

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main()
{
    mat4 view=rotationMatrix(vec3(1,1,1),time);


    vec4 positionH=vec4(0.25*vertexPosition,1);
    gl_Position =  view*positionH;
    color=vertexColor;
}
