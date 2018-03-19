#version 410
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColors;
uniform float time;
uniform mat4 MVP;
out vec4 color;
uniform bool deform;

void main()
{
  vec4 positionH = vec4(vertexPosition, 1);
  gl_Position = MVP * positionH;
  float r = length(gl_Position.xyz);
  if (deform) {
    gl_Position.xyz *= (1 + 0.2 * (r - 0.4) * cos(3 * time)) / 1.2;
  }
  color = vec4(vertexColors, 1);
}
