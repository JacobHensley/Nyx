#Shader Vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 binormals;
layout(location = 4) in vec2 texCoords;

out vec3 f_Normal;

uniform mat4 u_MVP;
uniform mat4 u_ModelMatrix;

void main()
{
	gl_Position = u_MVP * vec4(position, 1.0f);

	f_Normal = mat3(u_ModelMatrix) * normal;
}

#Shader Fragment

#version 330 core

layout(location = 0) out vec4 color;

struct Light
{
	vec3 Direction;
	vec3 Radiance;
};

in vec3 f_Normal;
uniform Light u_Light;
uniform int u_LightExponent;

uniform samplerCube u_TextureCube;

void main()
{
	vec3 light = -normalize(u_Light.Direction);
	vec3 norm = normalize(f_Normal);
	float brightness = dot(light, norm) * 0.5 + 0.5;
//	color = vec4(norm.xyz * 0.5 + 0.5, 1.0);
	if (u_LightExponent == 2)
		brightness = brightness * brightness;
	if (u_LightExponent == 3)
		brightness = brightness * brightness * brightness;
	color = texture(u_TextureCube, norm);
//	color = vec4(brightness * u_Light.Radiance.x, brightness * u_Light.Radiance.y, brightness * u_Light.Radiance.z, 1.0);
}