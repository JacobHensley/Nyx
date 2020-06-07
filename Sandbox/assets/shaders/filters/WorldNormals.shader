#Shader Vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormals;
layout(location = 4) in vec2 a_TexCoords;

layout(location = 8) out mat3 v_WorldNormals;

layout(std140, binding = 0) uniform r_RendererBuffer
{
	mat4 MVP;
	mat4 ModelMatrix;
} RendererBuffer;

void main()
{
	gl_Position = RendererBuffer.MVP * vec4(a_Position, 1.0f);

	v_WorldNormals = mat3(RendererBuffer.ModelMatrix) * mat3(a_Tangent, a_Binormals, a_Normal);
}

#Shader Fragment
#version 450 core

layout(location = 8) in mat3 v_WorldNormals;

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}