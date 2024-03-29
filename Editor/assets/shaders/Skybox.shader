#Shader Vertex
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec3 v_Position;

layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 ViewProjection;
	mat4 InverseVP;
} r_CameraBuffer;

void main()
{
	vec4 position = vec4(a_Position.xy, 1.0f, 1.0f);
	v_Position = (r_CameraBuffer.InverseVP * position).xyz;

	gl_Position = position;
}

#Shader Fragment
#version 450 core

layout(location = 0) in vec3 v_Position;
layout(location = 0) out vec4 color;

layout(location = 0) uniform bool u_UseIrradiance;

layout(binding = 1) uniform samplerCube r_SkyboxTextureRradiance;
layout(binding = 2) uniform samplerCube r_SkyboxTextureIrradiance;

void main()
{
	if (u_UseIrradiance)
		color = texture(r_SkyboxTextureIrradiance, v_Position);
	else
		color = texture(r_SkyboxTextureRradiance, v_Position);
}