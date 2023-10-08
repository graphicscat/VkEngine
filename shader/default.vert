#version 450
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (location = 3) in vec2 UV;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outViewPos;
layout (location = 2) out vec3 outFragPos;
layout (location = 3) out vec3 outNormal;


//push constants block
//layout( push_constant ) uniform constants
//{
//vec4 objectColor;
// mat4 render_matrix;
//} PushConstants;

layout(set = 0, binding = 0) uniform  CameraBuffer{   
	mat4 proj;    
	mat4 view;
	vec4 viewPos; 
	vec4 lightPos;
} cameraData;


void main() 
{	
	outFragPos = vec3(vec4(vPosition, 1.0f));
	gl_Position =cameraData.proj*cameraData.view * vec4(vPosition, 1.0f);
	outColor = vec3(0.1,0.2,0.5);
	outViewPos = vec3(cameraData.viewPos);
	outNormal = normalize(vNormal);
}