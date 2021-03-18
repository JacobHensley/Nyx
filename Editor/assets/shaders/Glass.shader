#Shader Vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormals;
layout(location = 4) in vec2 a_TexCoords;

layout(location = 5) out vec3 v_Normal;
layout(location = 6) out vec3 v_WorldPosition;
layout(location = 7) out vec2 v_TexCoords;
layout(location = 8) out mat3 v_WorldNormals;

// Index 0
layout(std140, binding = 0) uniform r_RendererBuffer
{
	mat4 ViewProjection;
	mat4 InverseVP;
} RendererBuffer;

layout(location = 0) uniform mat4 u_Transform;

void main()
{
	v_Normal = mat3(u_Transform) * a_Normal;
	v_WorldPosition = (u_Transform * vec4(a_Position, 1.0f)).xyz;
	v_TexCoords = a_TexCoords;
	v_WorldNormals = mat3(u_Transform) * mat3(a_Tangent, a_Binormals, a_Normal);

	gl_Position = RendererBuffer.ViewProjection * v_WorldPosition;
}

#Shader Fragment
#version 450 core

layout(location = 5) in vec3 v_Normal;
layout(location = 6) in vec3 v_WorldPosition;
layout(location = 7) in vec2 v_TexCoords;
layout(location = 8) in mat3 v_WorldNormals;

layout(location = 0) out vec4 color;

// Index 0
layout(std140, binding = 1) uniform m_UserBuffer
{
	vec3 AlbedoValue;
	bool UsingAlbedoMap;

	bool UsingNormalMap;

	float MetalnessValue;
	bool UsingMetalnessMap;

	float RoughnessValue;
	bool UsingRoughnessMap;

	float UsingIBL;
	float UsingLighting;
	
} UserBuffer;

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

// Index 1
layout(std140, binding = 2) uniform r_FragmentRendererBuffer
{
	vec3 CameraPosition;
	DirectionalLight DirectionLight;
	PointLight PLight;

} FragmentRendererBuffer;

layout(location = 0) uniform sampler2D r_BRDFLutTexture;
layout(location = 1) uniform samplerCube r_IrradianceTexture;
layout(location = 2) uniform samplerCube r_RadianceTexture;

void main()
{
	color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}