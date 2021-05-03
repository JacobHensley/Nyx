#Shader Vertex
#version 450 core

// Attributes
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

// Outputs
layout(location = 0) out vec3 v_NormalDirection;
layout(location = 1) out vec3 v_ViewDirection;

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

	mat4 inverseTransform = inverse(r_Renderer.Transform);
	v_NormalDirection = normalize(vec3(vec4(a_Normal, 0.0) * inverseTransform));
	v_ViewDirection = vec3(r_Renderer.Transform * vec4(a_Position, 1.0f) - vec4(r_CameraBuffer.CameraPosition, 1.0));

	gl_Position = MVP * vec4(a_Position, 1.0f);
}

#Shader Fragment
#version 450 core

// Outputs
layout(location = 0) out vec4 color;

// Inputs
layout(location = 0) in vec3 v_NormalDirection;
layout(location = 1) in vec3 v_ViewDirection;

// Renderer textures
layout(binding = 1) uniform samplerCube r_RadianceTexture;

void main()
{
	float refractiveIndex = 1.5;
	vec3 refractedDirection = refract(normalize(v_ViewDirection), normalize(v_NormalDirection), 1.0 / refractiveIndex);
	color = vec4(texture(r_RadianceTexture, refractedDirection).xyz * vec3(0.9f, 0.9f, 1.0f), 0.3f);
}