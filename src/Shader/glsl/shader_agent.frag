#version 460 core
layout (location = 0) out vec4 FragColor;

layout(location = 0) in vec3 inPos; 
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout (binding = 2) uniform sampler2D simple;
layout (binding = 3) uniform sampler2D clothSimple;

void main()
{
	vec3 n = normalize(inNormal);
	vec4 diffuse = vec4(0.0);
	vec4 specular = vec4(0.0);
	
	// the material properties are embedded in the shader (for now)
	vec4 mat_ambient = vec4(1.0, 1.0, 1.0, 1.0) * 10.0;
	vec4 mat_diffuse = vec4(1.0, 1.0, 1.0, 1.0) * 10.0;
	vec4 mat_specular = vec4(1.0, 1.0, 1.0, 1.0) * 10.0;
	
	// ambient term
	vec4 ambient = mat_ambient * 0.4;
	
	// diffuse color
	vec4 kd = mat_diffuse * 0.6;
	
	// specular color
	vec4 ks = mat_specular * 0.1;
	
	// diffuse term
	vec3 lightDir = normalize(vec3(50, 50, 0) - inPos);
	float NdotL = dot(n, lightDir);
	
	if (NdotL > 0.0)
		diffuse = kd * NdotL;
	
	// specular term
	vec3 rVector = normalize(2.0 * n * dot(n, lightDir) - lightDir);
	vec3 viewVector = normalize(-inPos);
	float RdotV = dot(rVector, viewVector);
	
	if (RdotV > 0.0)
		specular = ks * pow(RdotV, 0.6);
	if(texture(simple, inTexCoord).r == 1)
	{
		FragColor = (ambient + diffuse + specular) * vec4(1.0, 0.0, 0.0, 1.0);
	}
	else
	{
		FragColor = (ambient + diffuse + specular) * texture(simple, inTexCoord);
	}
}