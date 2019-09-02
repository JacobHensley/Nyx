#Shader Vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormals;
layout(location = 4) in vec2 a_TexCoords;

uniform mat4 u_MVP;
uniform mat4 u_ModelMatrix;

out vec3 v_Normal;
out vec3 v_WorldPos;

void main()
{
	gl_Position = u_MVP * vec4(a_Position, 1.0f);

	v_Normal = mat3(u_ModelMatrix) * a_Normal;
	v_WorldPos = vec3(u_ModelMatrix * vec4(a_Normal, 1.0f));
}

#Shader Fragment

#version 330 core

layout(location = 0) out vec4 color;

struct Light
{
	vec3 Direction;
	vec3 Radiance;
};

uniform Light u_Light;
uniform int u_LightExponent;
uniform vec3 u_CameraPos;
uniform samplerCube u_TextureCube;

in vec3 v_Normal;
in vec3 v_WorldPos;

void main()
{
	vec3 view = -normalize(u_CameraPos - v_WorldPos);
	vec3 light = -normalize(u_Light.Direction);
	vec3 norm = normalize(v_Normal);

	float brightness = dot(light, norm) * 0.5 + 0.5;
	brightness = pow(brightness, u_LightExponent);

	vec3 tc = reflect(view, norm);

	color = textureLod(u_TextureCube, tc, 8.0);
//	color = vec4(norm.xyz * 0.5 + 0.5, 1.0);
//	color = vec4(brightness * u_Light.Radiance.x, brightness * u_Light.Radiance.y, brightness * u_Light.Radiance.z, 1.0);
}