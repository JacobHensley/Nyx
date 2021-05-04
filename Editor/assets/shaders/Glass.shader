#Shader Vertex
#version 450 core

// Attributes
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

// Outputs
layout(location = 0) out vec3 v_WorldNormal;
layout(location = 1) out vec3 v_WorldPosition;

// Uniform buffer
layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 ViewProjection;
	mat4 InverseVP;
	vec3 CameraPosition;
} r_CameraBuffer;

// Renderer uniforms
struct Renderer
{
	mat4 Transform;
};

layout(location = 0) uniform Renderer r_Renderer;

//--------------------------------------------------------------------------------

void main()
{
	mat4 MVP = r_CameraBuffer.ViewProjection * r_Renderer.Transform;
	mat4 inverseTransform = inverse(r_Renderer.Transform);

	v_WorldNormal = inverse(transpose(mat3(r_Renderer.Transform))) * a_Normal;
	v_WorldPosition = vec3(r_Renderer.Transform * vec4(a_Position, 1.0f));

	gl_Position = MVP * vec4(a_Position, 1.0f);
}

#Shader Fragment
#version 450 core

// Inputs
layout(location = 0) in vec3 v_WorldNormal;
layout(location = 1) in vec3 v_WorldPosition;

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

// Renderer textures
layout(binding = 1) uniform samplerCube r_RadianceTexture;

//--------------------------------------------------------------------------------

vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(vec3 F0, float cosTheta, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float ndfGGX(float cosLh, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

float gaSchlickGGX(float cosLi, float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;
	return gaSchlickG1(cosLi, k) * gaSchlickG1(NdotV, k);
}

//--------------------------------------------------------------------------------

vec3 DirectionalLight_Contribution(vec3 F0, vec3 V, vec3 N, vec3 albedo, float R, float M, float NdotV, out vec3 specular)
{
	vec3 Li = -r_LightBuffer.DirectionLight.Direction;
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
	specular = specularBRDF;
	return (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
}

//--------------------------------------------------------------------------------

void main()
{
	vec3 albedo = vec3(0.7f, 0.8f, 0.9f);
	vec3 normal = normalize(v_WorldNormal);
	float metalness = 0.0f;
	float roughness = 0.1f;
	
	vec3 view = normalize(r_CameraBuffer.CameraPosition - v_WorldPosition);
	float NdotV = max(dot(normal, view), 0.0);
	vec3 Lr = 2.0 * NdotV * normal - view;

	const vec3 Fdielectric = vec3(0.04);

	float alpha = 0.5f;
	float refractiveIndex = 1.5f;
	float normalScale = 0.1f;

	vec3 specular;
	vec3 directionalLight_Contribution = DirectionalLight_Contribution(Fdielectric, view, normal, albedo, roughness, metalness, NdotV, specular);

	vec3 refractedDirection = refract(view, normalize(view + normal * normalScale), 1.0 / refractiveIndex);
	vec3 environment_Contribution = texture(r_RadianceTexture, refractedDirection).xyz;

	color.rgb = directionalLight_Contribution * alpha + environment_Contribution;
	color.a = max(alpha, specular.r);
}