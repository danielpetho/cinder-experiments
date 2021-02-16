#version 150

uniform float		uCheckSize;
uniform float		u_time;
in vec2				TexCoord0;
out vec4			oColor;

vec4 checker(vec2 uv)
{
	float v = floor(uCheckSize * uv.x) + floor(uCheckSize * uv.y);
	if (mod(v, 2.0) < 1.0)
		return vec4(abs(sin(u_time * 0.001f)), abs(sin(u_time * 0.01f)), 1, 1);
	else
		return vec4(0, 0, 0, 1);
}

void main(void) 
{
	oColor = checker(TexCoord0);
}