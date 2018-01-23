#version 410
in vec4 color;

out vec4 fragColor ; 
uniform float time;

void main()
{
    fragColor=color;
    fragColor.g *= (1+sin(time))*0.5;
}
