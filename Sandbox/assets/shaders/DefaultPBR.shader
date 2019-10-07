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

struct Light 
{
	vec3 radiance;
	vec3 direction;
};

uniform Light u_Light;

uniform vec3 u_CameraPosition;

uniform samplerCube u_IrradianceTexture;
uniform samplerCube u_RadianceTexture;

uniform sampler2D u_BRDFLutTexture;

// Albedo uniforms
uniform vec3 u_AlbedoValue;
uniform sampler2D u_AlbedoMap;
uniform bool u_UsingAlbedoMap;

// Normal uniforms
uniform sampler2D u_NormalMap;
uniform bool u_UsingNormalMap;

// Metalness uniforms
uniform float u_MetalnessValue;
uniform sampler2D u_MetalnessMap;
uniform bool u_UsingMetalnessMap;

// Roughness uniforms
uniform float u_RoughnessValue;
uniform sampler2D u_RoughnessMap;
uniform bool u_UsingRoughnessMap;

in vec3 v_Normal;
in vec3 v_WorldPosition;
in vec2 v_TexCoords;

vec3 GetAlbedo(vec2 texCoords)
{
	return u_UsingAlbedoMap ? texture(u_AlbedoMap, texCoords).rgb : u_AlbedoValue;
}

vec3 GetNormal(vec2 texCoords, vec3 normal)
{
	if (u_UsingNormalMap)
		return texture(u_NormalMap, texCoords).rgb * 2.0 - 1.0;

	return normalize(normal);
}

float GetMetalness(vec2 texCoords)
{
	return u_UsingMetalnessMap ? texture(u_MetalnessMap, texCoords).r : u_MetalnessValue;
}

float GetRoughness(vec2 texCoords)
{
	return u_UsingRoughnessMap ? texture(u_RoughnessMap, texCoords).r : u_RoughnessValue;
}

void main()
{

	// Process inputs (extract everything we need): albedo, normals, metalness, roughness
	vec3 albedo = GetAlbedo(v_TexCoords);
	vec3 normal = GetNormal(v_TexCoords, v_Normal);
	float metalness = GetMetalness(v_TexCoords);
	float roughness = GetRoughness(v_TexCoords); // Note: maybe set min to ~0.05

	vec3 view = -normalize(u_CameraPosition - v_WorldPosition);
	float NdotV = max(dot(normal, view), 0.0);

	// I - 2.0 * dot(N, I) * N.
	vec3 Lr = view - 2.0 * NdotV * normal;

	//  Constant normal incidence Fresnel factor for all dielectrics.
	const vec3 Fdielectric = vec3(0.04);

	// Fresnel reflectance, metals use albedo
	vec3 F0 = mix(Fdielectric, albedo, metalness);

	int mipLevels = textureQueryLevels(u_RadianceTexture);

//	float roughness = sqrt(texture(u_RoughnessMap, v_TexCoords).x) * mipLevels;

//	color = texture(u_MetalnessMap, v_TexCoords);
    color = vec4(1.0f, 0.0f, 1.0f, 1.0f) * vec4(u_Light.radiance.xyz, 1.0f);
//  color = textureLod(u_RadianceTexture, tc, roughness);
	vec3 tc = reflect(view, normal);
//	color = textureLod(u_RadianceTexture, tc, roughness) * texture(u_AlbedoMap, v_TexCoords);
}