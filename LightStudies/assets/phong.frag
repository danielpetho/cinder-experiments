#version 330

uniform sampler2D	uTex0;

uniform vec3		uLightPos;
uniform vec3		uLightColor;
uniform float		uAmbientStrength;
uniform float		uSpecularStrength;
uniform float		uMaterialShininess;
uniform vec3		uViewPos;

in VertexData {
	vec4 position;
	vec3 normal;
	vec4 color;
	vec2 texCoord;
} vVertexIn;

out vec4 oFragColor;

void main() {
	//vec3 lightColor = vec3(1.0);
	vec3 lightColor = uLightColor;
	//vec3 vLightPosition = vec3( 0.0 );
	vec3 lightPosition = uLightPos;

	// ambient
	float ambientStrength = uAmbientStrength;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse 
	vec3 N = normalize( vVertexIn.normal );
	vec3 L = normalize( lightPosition - vVertexIn.position.xyz );
	float diff = max( dot( N, L ), 0.0 );
	vec3 diffuse = diff * lightColor;

	// specular
	float specularStrength = uSpecularStrength;
	vec3 viewDir = normalize(uViewPos - vVertexIn.position.xyz);
	vec3 reflectDir = reflect(-L, N);
	float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), uMaterialShininess);
	vec3 specular = specularStrength * spec * lightColor;

	//vec3 result = (ambient + diffuse + specular) * texture( uTex0, vVertexIn.texCoord.st ).rgb;
	vec3 result = (ambient + diffuse + specular) * vec3(1.0);
	oFragColor = vec4(result, 1.0);

}