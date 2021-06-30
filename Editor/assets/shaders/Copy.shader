#Shader Vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

layout(location = 0) out vec2 v_TexCoords;

void main()
{
	gl_Position = vec4(a_Position, 1.0f);
	v_TexCoords = a_TexCoords;
}

#Shader Fragment
#version 450 core

layout(location = 0) in vec2 v_TexCoords;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D r_InputTexture;

void main()
{
	color = vec4(texture(r_InputTexture, v_TexCoords));
}