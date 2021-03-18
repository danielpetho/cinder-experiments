#version 330 core

uniform mat4	ciModelViewProjection;
uniform mat4	ciModelView;
uniform mat4	ciView;
uniform mat4	ciProjection;
uniform mat4	ciModel;
uniform mat3	ciNormalMatrix;
uniform vec3	uViewPos;

in vec4		ciPosition;
in vec3		ciNormal;
in vec4		ciColor;
in vec2		ciTexCoord0;

out VertexData {
	vec4 position;
	vec3 normal;
	vec4 color;
	vec2 texCoord;
} vVertexOut;

void main()
{
	vVertexOut.position = ciModel * ciPosition;
	vVertexOut.normal = ciNormal;
	vVertexOut.color = ciColor;
	vVertexOut.texCoord = ciTexCoord0;

	gl_Position = ciModelViewProjection * ciPosition;
}