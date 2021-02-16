#version 150

uniform mat4	ciModelViewProjection;
uniform float   u_time;
in vec4			ciPosition;
in vec2			ciTexCoord0;
out vec2		TexCoord0;

float offset(vec2 uv)
{
	return (sin(uv.x * 15.0 + u_time * 100.0f) +
		cos(uv.y * 7.0f + u_time + uv.x * 13.0f + u_time)) * 0.1f;
}

void main(void) {
	vec4 pos = ciPosition;
	pos.y = offset(ciTexCoord0);
	gl_Position = ciModelViewProjection * pos;
	TexCoord0 = ciTexCoord0;
}