//glsl version 4.5
#version 450

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inViewPos;
layout (location = 2) in vec3 inFragPos;
layout (location = 3) in vec3 inNormal;

//output write
layout (location = 0) out vec4 outFragColor;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}light;

void main() 
{
	light.position = vec3(0.0,5.0,3.0);
	light.ambient = vec3(0.2);
	light.diffuse = vec3(0.8);
	light.specular = vec3(1.0);
	vec3 lightDir = normalize(light.position - inFragPos);
	vec3 viewDir = normalize(inViewPos - inFragPos);
	
	float diff = max(dot(lightDir,inNormal),0.0);
	vec3 diffuse = diff * inColor;
	vec3 amb = 0.1*inColor;
	
	vec3 result = amb + diffuse;
	
	
	//return red
	//result = pow(result,vec3(0.454));
	outFragColor = vec4(result,1.0);
}