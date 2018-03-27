#version 410
in vec2 uv;
out vec4 fragColor;
uniform sampler2D fontSampler;
uniform vec3 fontColor;
uniform vec4 fillColor;

void main()
{
  fragColor = texture(fontSampler, uv);
  fragColor.a = fragColor.r;
  if(fragColor.a == 0) {
    fragColor = fillColor;
  }
  else {
    fragColor.rgb = fontColor;
  }
}
