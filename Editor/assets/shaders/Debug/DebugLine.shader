#Shader Vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

layout(binding = 0) uniform r_RendererBuffer
{
	mat4 VP;
} RendererBuffer;


layout(location = 3) out vec4 v_Color;

void main()
{
	v_Color = a_Color;

	gl_Position = RendererBuffer.VP * vec4(a_Position, 1.0);
}

#Shader Fragment
#version 450 core

layout(location = 0) out vec4 color;

layout(location = 3) in vec4 v_Color;

void main()
{
	color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}