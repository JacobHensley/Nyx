#Shader Vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 ViewProjection;
	mat4 InverseVP;
} r_CameraBuffer;

layout(location = 0) uniform mat4 r_Transform;

void main()
{
	mat4 MVP = r_CameraBuffer.ViewProjection * r_Transform;
	gl_Position = MVP * vec4(a_Position, 1.0f);
}

#Shader Fragment
#version 450 core

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}