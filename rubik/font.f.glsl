#version 410
in vec2 uv;
out vec4 fragColor;
uniform sampler2D fontSampler;
uniform vec3 fontColor;

void main()
{
  fragColor = texture(fontSampler, uv);
  fragColor.a = fragColor.r;
  fragColor.rgb = fontColor;
}
