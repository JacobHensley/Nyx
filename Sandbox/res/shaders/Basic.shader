#Shader Vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * vec4(position, 1.0f);
}

#Shader Fragment

#version 330 core

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1.0f, 1.0, 1.0f, 1.0);
}