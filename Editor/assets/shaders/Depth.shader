#Shader Vertex
#version 450 core

// Attributes
layout(location = 0) in vec3 a_Position;

// Uniform buffer
layout(std140, binding = 0) uniform CameraBuffer
{
	mat4 ViewProjection;
	mat4 InverseVP;
	vec3 CameraPosition;
} r_CameraBuffer;

// Renderer uniforms
struct Renderer
{
	mat4 Transform;
};

layout(location = 0) uniform Renderer r_Renderer;

void main()
{
	mat4 MVP = r_CameraBuffer.ViewProjection * r_Renderer.Transform;

	gl_Position = MVP * vec4(a_Position, 1.0f);
}

#Shader Fragment
#version 450 core

// Outputs
layout(location = 0) out vec4 color;

void main()
{
}