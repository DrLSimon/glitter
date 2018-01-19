#version 410

in vec2 uv ;
out vec4 fragColor ; 
uniform sampler2D colorSampler;

void main()
{
    fragColor=texture(colorSampler, uv);
}
