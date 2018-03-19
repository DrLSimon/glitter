#version 410
layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexUV;
out vec2 uv;
uniform float wOverH;

void main()
{
  vec4 positionH = vec4(vertexPosition, -1, 1);
  gl_Position = positionH;
  if (wOverH < 1) {
    gl_Position.y *= wOverH;
  } else {
    gl_Position.x /= wOverH;
  }
  uv = vertexUV;
}
