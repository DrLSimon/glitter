#version 410

out vec4 fragColor ; 
uniform float time;

void main()
{
    fragColor=vec4(1,(1+sin(time))*0.5,0,1);
}
