#Shader Vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out vec3 f_Normal;

uniform mat4 u_MVP;
uniform mat4 u_ModelMatrix;

void main()
{
	gl_Position = u_MVP * vec4(position, 1.0f);

	f_Normal = mat3(transpose(inverse(u_ModelMatrix))) * normal;
}

#Shader Fragment

#version 330 core

layout(location = 0) out vec4 color;

in vec3 f_Normal;

void main()
{
	vec3 norm = normalize(f_Normal);
	color = vec4(norm.xyz * 0.5 + 0.5, 1.0);
}