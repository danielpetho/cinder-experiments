#version 150

in vec2				TexCoord0;
out vec4			oColor;

void main(void) 
{
	oColor = vec4(TexCoord0.x, 0.0, TexCoord0.y, 1.0);
}