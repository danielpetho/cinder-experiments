#version 330

struct Material {
    vec3 diffuse;
    vec3 specular;
	vec3 ambient;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};
#define NR_POINT_LIGHTS 1  
#define NR_DIR_LIGHTS 1
#define NR_SPOT_LIGHTS 1

uniform vec3			uViewPos;

uniform DirLight		dirLights[NR_DIR_LIGHTS];
uniform PointLight		pointLights[NR_POINT_LIGHTS];
uniform SpotLight		spotLights[NR_SPOT_LIGHTS];
uniform Material		material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

//------------------------------------------------
// VERTEX data

uniform sampler2D	uTex0;

in VertexData	{
	vec4 position;
	vec3 normal;
	vec4 color;
	vec2 texCoord;
} vVertexIn;

out vec4 oFragColor;

void main() {

    // properties
    vec3 norm = normalize(vVertexIn.normal);
    vec3 viewDir = normalize(uViewPos - vVertexIn.position.xyz);
    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting

	vec3 result;// = CalcDirLight(dirLights[0], norm, viewDir);
	/*for(int i = 0; i < NR_DIR_LIGHTS; i++)
        result += CalcDirLight(dirLights[i], norm, viewDir);    */
    
    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, vVertexIn.position.xyz, viewDir);    
    // phase 3: spot light
	//for(int i = 0; i < NR_SPOT_LIGHTS; i++)
    //	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    oFragColor = vec4(result, 1.0);

}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
	// combine results

	//texture materials
    /*vec3 ambient = light.ambient * vec3(texture(material.diffuse,  vVertexIn.texCoord.st));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,  vVertexIn.texCoord.st));
    vec3 specular = light.specular * spec * vec3(texture(material.specular,  vVertexIn.texCoord.st));*/

	// color materials
	vec3 ambient = light.ambient * vec3(material.ambient);
    vec3 diffuse = light.diffuse * diff * vec3(material.diffuse);
    vec3 specular = light.specular * spec * vec3(material.specular);
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    


    // combine results

	// texture materials
    /*vec3 ambient = light.ambient * vec3(texture(material.diffuse,  vVertexIn.texCoord.st));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,  vVertexIn.texCoord.st));
    vec3 specular = light.specular * spec * vec3(texture(material.specular,  vVertexIn.texCoord.st));*/


	// color material
	vec3 ambient = light.ambient * vec3(material.ambient);
    vec3 diffuse = light.diffuse * diff * vec3(material.diffuse);
    vec3 specular = light.specular * spec * vec3(material.specular);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results

	// texture materials
    /*vec3 ambient = light.ambient * vec3(texture(material.diffuse,  vVertexIn.texCoord.st));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,  vVertexIn.texCoord.st));
    vec3 specular = light.specular * spec * vec3(texture(material.specular,  vVertexIn.texCoord.st));*/
    
	// color materials
	vec3 ambient = light.ambient * vec3(material.ambient);
    vec3 diffuse = light.diffuse * diff * vec3(material.diffuse);
    vec3 specular = light.specular * spec * vec3(material.specular);

	ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
