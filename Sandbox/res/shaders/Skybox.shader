#Shader Vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_InverseVP;

out vec3 v_Position;

void main()
{
	vec4 position = vec4(a_Position.xy, 1.0, 1.0);
	v_Position = (u_InverseVP * position).xyz;

	gl_Position = position;
}

#Shader Fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform samplerCube u_SkyboxTexture;

in vec3 v_Position;

void main()
{
	color = texture(u_SkyboxTexture, v_Position);
}