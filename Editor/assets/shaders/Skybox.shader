#Shader Vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 2) out vec3 v_Position;

layout(std140, binding = 0) uniform r_RendererBuffer
{
	mat4 ViewProjection;
	mat4 InverseVP;
} RendererBuffer;

void main()
{
	vec4 position = vec4(a_Position.xy, 1.0, 1.0);
	v_Position = (RendererBuffer.InverseVP * position).xyz;

	gl_Position = position;
}

#Shader Fragment
#version 450 core

layout(location = 2) in vec3 v_Position;

uniform samplerCube u_SkyboxTexture;

layout(location = 0) out vec4 color;

void main()
{
	color = texture(u_SkyboxTexture, v_Position);
}