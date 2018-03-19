#version 410
in vec2 uv;
out vec4 fragColor;
uniform sampler2D colormap;

void main()
{
  fragColor = texture(colormap, uv);
}
