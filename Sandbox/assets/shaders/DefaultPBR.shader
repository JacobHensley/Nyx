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
out mat3 v_WorldNormals;

void main()
{
	gl_Position = u_MVP * vec4(a_Position, 1.0f);

	v_Normal = mat3(u_ModelMatrix) * a_Normal;
	v_WorldPosition = vec3(u_ModelMatrix * vec4(a_Position, 1.0f));
	v_TexCoords = a_TexCoords;
	v_WorldNormals = mat3(u_ModelMatrix) * mat3(a_Tangent, a_Binormals, a_Normal);
}

#Shader Fragment
#version 430 core

layout(location = 0) out vec4 color;

const float Epsilon = 0.00001;
const float PI = 3.141592;

uniform vec3 u_Radiance;
uniform vec3 u_Direction;

uniform vec3 u_CameraPosition;

uniform bool u_UsingIBL;
uniform bool u_UsingLighting;

// Albedo uniforms
uniform vec3 u_AlbedoValue;
uniform bool u_UsingAlbedoMap;

// Normal uniforms
uniform bool u_UsingNormalMap;

// Metalness uniforms
uniform float u_MetalnessValue;
uniform bool u_UsingMetalnessMap;

// Roughness uniforms
uniform float u_RoughnessValue;
uniform bool u_UsingRoughnessMap;

uniform sampler2D u_NormalMap;
uniform sampler2D u_AlbedoMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_MetalnessMap;
uniform sampler2D u_BRDFLutTexture;
uniform samplerCube u_IrradianceTexture;
uniform samplerCube u_RadianceTexture;

in vec3 v_Normal;
in vec3 v_WorldPosition;
in vec2 v_TexCoords;
in mat3 v_WorldNormals;

vec3 GetAlbedo(vec2 texCoords)
{
	return u_UsingAlbedoMap ? texture(u_AlbedoMap, texCoords).rgb : u_AlbedoValue;
}

vec3 GetNormal(vec2 texCoords, vec3 normal)
{
	if (u_UsingNormalMap) 
		return normalize(v_WorldNormals * (texture(u_NormalMap, texCoords).rgb * 2.0 - 1.0));

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

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(vec3 F0, float cosTheta, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2
float ndfGGX(float cosLh, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(NdotV, k);
}

vec3 Lighting(vec3 F0, vec3 V, vec3 N, vec3 albedo, float R, float M, float NdotV)
{
	vec3 Li = -u_Direction;
	vec3 Lradiance = u_Radiance;
	vec3 Lh = normalize(Li + V);

	float cosLi = max(0.0, dot(N, Li));
	float cosLh = max(0.0, dot(N, Lh));

	vec3 F = fresnelSchlick(F0, max(0.0, dot(Lh, V)));
	float D = ndfGGX(cosLh, R);
	float G = gaSchlickGGX(cosLi, NdotV, R);

	vec3 kd = (1.0 - F) * (1.0 - M);
	vec3 diffuseBRDF = kd * albedo;

	vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * NdotV);

	return (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
}

vec3 IBL(vec3 Lr, vec3 albedo, float R, float M, vec3 N, vec3 V, float NdotV, vec3 F0)
{
	vec3 irradiance = texture(u_IrradianceTexture, N).rgb;
	vec3 F = fresnelSchlickRoughness(F0, NdotV, R);
	vec3 kd = (1.0 - F) * (1.0 - M);
	vec3 diffuseIBL = albedo * irradiance;

	int mipLevels = textureQueryLevels(u_RadianceTexture);
	float roughness = sqrt(R) * mipLevels;
	vec3 specularIrradiance = textureLod(u_RadianceTexture, Lr, roughness).rgb * albedo;

	vec2 specularBRDF = texture(u_BRDFLutTexture, vec2(NdotV, 1.0 - R)).rg;
	vec3 specularIBL = specularIrradiance * (F * specularBRDF.x + specularBRDF.y);
	return kd * diffuseIBL + specularIBL;
}

void main()
{
	vec3 albedo = GetAlbedo(v_TexCoords);
	vec3 normal = GetNormal(v_TexCoords, v_Normal);
	float metalness = GetMetalness(v_TexCoords);
	float roughness = GetRoughness(v_TexCoords);

	vec3 view = normalize(u_CameraPosition - v_WorldPosition);
	float NdotV = max(dot(normal, view), 0.0);

	// I - 2.0 * dot(N, I) * N.
	// vec3 Lr = view - 2.0 * NdotV * normal;
	vec3 Lr = 2.0 * NdotV * normal - view;

	// Constant normal incidence Fresnel factor for all dielectrics.
	const vec3 Fdielectric = vec3(0.04);

	// Fresnel reflectance, metals use albedo
	vec3 F0 = mix(Fdielectric, albedo, metalness);

	vec3 lightContribution = Lighting(F0, view, normal, albedo, roughness, metalness, NdotV);
	vec3 iblContribution = IBL(Lr, albedo, roughness, metalness, normal, view, NdotV, F0);

	color = vec4(lightContribution + iblContribution, 1.0f);
}