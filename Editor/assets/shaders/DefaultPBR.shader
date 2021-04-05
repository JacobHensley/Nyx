#Shader Vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormals;
layout(location = 4) in vec2 a_TexCoords;

layout(location = 0) out vec3 v_Normal;
layout(location = 1) out vec3 v_WorldPosition;
layout(location = 2) out vec2 v_TexCoords;
layout(location = 3) out mat3 v_WorldNormals;

layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 ViewProjection;
	mat4 InverseVP;
} r_CameraBuffer;

layout(location = 0) uniform mat4 r_Transform;

void main()
{
	mat4 MVP = r_CameraBuffer.ViewProjection * r_Transform;
	gl_Position = MVP * vec4(a_Position, 1.0f);

	v_Normal = mat3(r_Transform) * a_Normal;
	v_WorldPosition = vec3(r_Transform * vec4(a_Position, 1.0f));
	v_TexCoords = a_TexCoords;
	v_WorldNormals = mat3(r_Transform) * mat3(a_Tangent, a_Binormals, a_Normal);
}

#Shader Fragment
#version 450 core

layout(location = 0) in vec3 v_Normal;
layout(location = 1) in vec3 v_WorldPosition;
layout(location = 2) in vec2 v_TexCoords;
layout(location = 3) in mat3 v_WorldNormals;

layout(location = 0) out vec4 color;

const float Epsilon = 0.00001;
const float PI = 3.141592;

struct DirectionalLight
{
	vec3 radiance;
	vec3 direction;

	float lightActive;
};

struct PointLight
{
	vec3 position;
	vec4 radiance; // rgb = color, a = intensity
};


layout(location = 1) uniform vec3 u_AlbedoValue;
layout(location = 2) uniform bool u_UsingAlbedoMap;

layout(location = 3) uniform float u_MetalnessValue;
layout(location = 4) uniform bool u_UsingMetalnessMap;

layout(location = 5) uniform float u_RoughnessValue;
layout(location = 6) uniform bool u_UsingRoughnessMap;

layout(location = 7) uniform bool u_UsingNormalMap;

layout(location = 8) uniform float u_UsingIBL;
layout(location = 9) uniform float u_UsingLighting;

layout(location = 10) uniform vec3 r_CameraPosition;

layout(location = 11) uniform DirectionalLight r_DirectionLight;
layout(location = 14) uniform PointLight r_PointLight;

layout(location = 16) uniform sampler2D r_BRDFLutTexture;
layout(location = 17) uniform samplerCube r_IrradianceTexture;
layout(location = 18) uniform samplerCube r_RadianceTexture;

layout(location = 19) uniform sampler2D u_NormalMap;
layout(location = 20) uniform sampler2D u_AlbedoMap;
layout(location = 21) uniform sampler2D u_RoughnessMap;
layout(location = 22) uniform sampler2D u_MetalnessMap;

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
	vec3 Li = -r_DirectionLight.direction;
	vec3 Lradiance = r_DirectionLight.radiance;
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

vec3 PointLight_Contribution(vec3 F0, vec3 V, vec3 N, vec3 albedo, float R, float M, float NdotV)
{
	vec3 Li = normalize(r_PointLight.position - v_WorldPosition);
	vec3 Lh = normalize(Li + V);

	float distance = length(r_PointLight.position - v_WorldPosition);
	float attenuation = 1.0 / (distance * distance);
	vec3 Lradiance = r_PointLight.radiance.rgb * r_PointLight.radiance.a * attenuation;

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
	vec3 irradiance = texture(r_IrradianceTexture, N).rgb;
	vec3 F = fresnelSchlickRoughness(F0, NdotV, R);
	vec3 kd = (1.0 - F) * (1.0 - M);
	vec3 diffuseIBL = albedo * irradiance;

	int mipLevels = textureQueryLevels(r_RadianceTexture);
	float roughness = sqrt(R) * mipLevels;
	vec3 specularIrradiance = textureLod(r_RadianceTexture, Lr, roughness).rgb * albedo;

	vec2 specularBRDF = texture(r_BRDFLutTexture, vec2(NdotV, 1.0 - R)).rg;
	vec3 specularIBL = specularIrradiance * (F * specularBRDF.x + specularBRDF.y);
	return kd * diffuseIBL + specularIBL;
}

void main()
{
	vec3 albedo = GetAlbedo(v_TexCoords);
	vec3 normal = GetNormal(v_TexCoords, v_Normal);
	float metalness = GetMetalness(v_TexCoords);
	float roughness = GetRoughness(v_TexCoords);

	vec3 view = normalize(r_CameraPosition - v_WorldPosition);
	float NdotV = max(dot(normal, view), 0.0);

	// I - 2.0 * dot(N, I) * N.
	// vec3 Lr = view - 2.0 * NdotV * normal;
	vec3 Lr = 2.0 * NdotV * normal - view;

	// Constant normal incidence Fresnel factor for all dielectrics.
	const vec3 Fdielectric = vec3(0.04);

	// Fresnel reflectance, metals use albedo
	vec3 F0 = mix(Fdielectric, albedo, metalness);

	vec3 lightContribution = Lighting(F0, view, normal, albedo, roughness, metalness, NdotV);
	vec3 pLightContribution = PointLight_Contribution(F0, view, normal, albedo, roughness, metalness, NdotV);
	vec3 iblContribution = IBL(Lr, albedo, roughness, metalness, normal, view, NdotV, F0);

	color = vec4(pLightContribution + lightContribution + iblContribution, 1.0f);
	color = vec4(u_AlbedoValue.rgb, 1.0f);
}