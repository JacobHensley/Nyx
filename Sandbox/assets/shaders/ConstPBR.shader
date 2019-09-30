#Shader Vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormals;
layout(location = 4) in vec2 a_TexCoords;

uniform mat4 u_MVP;
uniform mat4 u_ModelMatrix;

out vec3 v_Normal;
out vec3 v_WorldPosition;
out vec2 v_TexCoords;

void main()
{
	gl_Position = u_MVP * vec4(a_Position, 1.0f);

	v_Normal = mat3(u_ModelMatrix) * a_Normal;
	v_WorldPosition = vec3(u_ModelMatrix * vec4(a_Position, 1.0f));
	v_TexCoords = a_TexCoords;
}

#Shader Fragment

#version 430 core

layout(location = 0) out vec4 color;

uniform vec3 u_CameraPosition;

uniform samplerCube u_IrradianceTexture;
uniform samplerCube u_RadianceTexture;

uniform sampler2D u_BRDFLutTexture;

uniform vec3 u_Albedo;
uniform float u_Metalness;
uniform float u_Normal;
uniform float u_Roughness;

in vec3 v_Normal;
in vec3 v_WorldPosition;
in vec2 v_TexCoords;

void main()
{
	vec3 view = -normalize(u_CameraPosition - v_WorldPosition);
	vec3 normal = normalize(v_Normal);

	float NdotV = max(dot(normal, view), 0.0);
	vec3 tc = reflect(view, normal);

	int mipLevels = textureQueryLevels(u_RadianceTexture);

	float roughness = sqrt(u_Roughness) * mipLevels;

	//	color = texture(u_MetalnessMap, v_TexCoords);
	//	color = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	//	color = textureLod(u_RadianceTexture, tc, roughness);
	color = textureLod(u_RadianceTexture, tc, roughness) * vec4(u_Albedo.xyz, 1.0f);
}