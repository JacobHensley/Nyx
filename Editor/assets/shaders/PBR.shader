#Shader Vertex
#version 450 core

// Attributes
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormals;
layout(location = 4) in vec2 a_TexCoords;

// Outputs
layout(location = 0) out vec3 v_Normal;
layout(location = 1) out vec3 v_WorldPosition;
layout(location = 2) out vec4 v_LightSpacePosition;
layout(location = 3) out vec2 v_TexCoords;
layout(location = 4) out vec3 v_WorldNormals;

// Uniform buffers
layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 ViewProjection;
	mat4 InverseVP;
	vec3 CameraPosition;
} r_CameraBuffer;

layout(std140, binding = 2) uniform ShadowBuffer
{
	mat4 ShadowMapViewProjection;
} r_ShadowBuffer;

// Renderer uniforms
struct Renderer
{
	mat4 Transform;
};

layout(location = 0) uniform Renderer r_Renderer;

//--------------------------------------------------------------------------------

void main()
{
	// Calculate vertex screen position
	mat4 MVP = r_CameraBuffer.ViewProjection * r_Renderer.Transform;
	gl_Position = MVP * vec4(a_Position, 1.0f);

	// Calculate other vertex data
	v_Normal = mat3(r_Renderer.Transform) * a_Normal;
	v_WorldPosition = vec3(r_Renderer.Transform * vec4(a_Position, 1.0f));
	v_TexCoords = a_TexCoords;
//	v_WorldNormals = mat3(r_Renderer.Transform) * mat3(a_Tangent, a_Binormals, a_Normal);
	v_WorldNormals = inverse(transpose(mat3(r_Renderer.Transform))) * a_Normal;

	v_LightSpacePosition = r_ShadowBuffer.ShadowMapViewProjection * r_Renderer.Transform * vec4(a_Position, 1.0f);
}

#Shader Fragment
#version 450 core
//--------------------------------------------------------------------------------

// Inputs
layout(location = 0) in vec3 v_Normal;
layout(location = 1) in vec3 v_WorldPosition;
layout(location = 2) in vec4 v_LightSpacePosition;
layout(location = 3) in vec2 v_TexCoords;
layout(location = 4) in vec3 v_WorldNormals;

// Outputs
layout(location = 0) out vec4 color;

// Types
struct DirectionalLight
{
	vec3 Radiance;
	vec3 Direction;
};

struct PointLight
{
	vec3 Position;
	vec3 Radiance;
	float Intensity;
};

// Uniform Buffers
layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 ViewProjection;
	mat4 InverseVP;
	vec3 CameraPosition;
} r_CameraBuffer;

layout(std140, binding = 1) uniform LightBuffer
{
	DirectionalLight DirectionLight;
	PointLight PLight;
} r_LightBuffer;

// Constants
const float Epsilon = 0.00001;
const float PI = 3.141592;

// Material uniforms
struct Material
{
	vec3 AlbedoValue;
	float MetalnessValue;
	float RoughnessValue;

	bool UseNormalMap;
};

layout(location = 1) uniform Material u_Material;
layout(location = 10) uniform float u_MaxShadowDistance;
layout(location = 11) uniform float u_ShadowFade;

// Material textures
uniform sampler2D u_AlbedoMap;
uniform sampler2D u_MetalnessMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_NormalMap;

// Renderer textures
layout(binding = 0) uniform sampler2D r_BRDFLutTexture;
layout(binding = 1) uniform samplerCube r_RadianceTexture;
layout(binding = 2) uniform samplerCube r_IrradianceTexture;
layout(binding = 3) uniform sampler2D r_ShadowMap;

//--------------------------------------------------------------------------------

vec3 GetAlbedo(vec2 texCoords)
{
	return texture(u_AlbedoMap, texCoords).rgb * u_Material.AlbedoValue;
}

float GetMetalness(vec2 texCoords)
{
	return texture(u_MetalnessMap, texCoords).r * u_Material.MetalnessValue;
}

float GetRoughness(vec2 texCoords)
{
	return texture(u_RoughnessMap, texCoords).r * u_Material.RoughnessValue;
}

vec3 GetNormal(vec2 texCoords, vec3 normal)
{
	if (u_Material.UseNormalMap)
		return normalize(v_WorldNormals * (texture(u_NormalMap, texCoords).rgb * 2.0 - 1.0));

	return normalize(v_WorldNormals);
}

//--------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------

vec3 DirectionalLight_Contribution(vec3 F0, vec3 V, vec3 N, vec3 albedo, float R, float M, float NdotV)
{
	vec3 Li = r_LightBuffer.DirectionLight.Direction;
	vec3 Lradiance = r_LightBuffer.DirectionLight.Radiance;
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
	vec3 Li = normalize(r_LightBuffer.PLight.Position - v_WorldPosition);
	vec3 Lh = normalize(Li + V);

	float distance = length(r_LightBuffer.PLight.Position - v_WorldPosition);
	float attenuation = 1.0 / (distance * distance);
	vec3 Lradiance = r_LightBuffer.PLight.Radiance * r_LightBuffer.PLight.Intensity * attenuation;

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

vec3 IBL_Contribution(vec3 Lr, vec3 albedo, float R, float M, vec3 N, vec3 V, float NdotV, vec3 F0)
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

float ShadowCalculation(vec3 shadowCoords, float shadowFade)
{
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(r_ShadowMap, shadowCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = shadowCoords.z;
	// check whether current frag pos is in shadow
	float bias = max(0.002 * (1.0 - dot(v_WorldNormals, r_LightBuffer.DirectionLight.Direction)), 0.002);
	float shadow = step(closestDepth + bias, currentDepth);

	return shadow * shadowFade;
}

//--------------------------------------------------------------------------------

void main()
{
	vec3 albedo = GetAlbedo(v_TexCoords);
	vec3 normal = GetNormal(v_TexCoords, v_Normal);
	float metalness = GetMetalness(v_TexCoords);
	float roughness = GetRoughness(v_TexCoords); 

	vec3 view = normalize(r_CameraBuffer.CameraPosition - v_WorldPosition);
	float NdotV = max(dot(normal, view), 0.0);
	vec3 Lr = 2.0 * NdotV * normal - view;

	// Constant normal incidence Fresnel factor for all dielectrics.
	const vec3 Fdielectric = vec3(0.04);

	// Fresnel reflectance, metals use albedo
	vec3 F0 = mix(Fdielectric, albedo, metalness);

	// Calulate light contributions
	vec3 directionalLight_Contribution = DirectionalLight_Contribution(F0, view, normal, albedo, roughness, metalness, NdotV);
	vec3 pointLight_Contribution = PointLight_Contribution(F0, view, normal, albedo, roughness, metalness, NdotV);
	vec3 IBL_Contribution = IBL_Contribution(Lr, albedo, roughness, metalness, normal, view, NdotV, F0);

	// Calulate shadow data
	vec3 shadowCoords = v_LightSpacePosition.xyz / v_LightSpacePosition.w;
	shadowCoords = shadowCoords * 0.5 + 0.5;
	float distance = length(r_CameraBuffer.CameraPosition - v_WorldPosition);
	float shadowFade = 1.0f - smoothstep(u_MaxShadowDistance - u_ShadowFade, u_MaxShadowDistance, distance);

	float shadow = 1.0f - ShadowCalculation(shadowCoords, shadowFade);
	
	color = vec4(directionalLight_Contribution * shadow + IBL_Contribution, 1.0f);
}