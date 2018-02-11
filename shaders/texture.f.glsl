#version 410

in vec2 uv ;
out vec4 fragColor ; 
uniform sampler2D colorSampler;
uniform vec3 diffuseColor;

void main()
{
    fragColor=vec4(diffuseColor,1)*texture(colorSampler, uv);
}
