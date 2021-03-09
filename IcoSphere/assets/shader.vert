#version 150

uniform mat4	ciModelViewProjection;
uniform float   uTime;
uniform vec2	uResolution;
in vec4			ciPosition;


void main()
{
	vec4 pos = ciPosition;
	gl_Position = ciModelViewProjection * pos;
};