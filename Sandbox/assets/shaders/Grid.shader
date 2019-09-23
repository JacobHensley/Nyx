#Shader Vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

uniform mat4 u_MVP;

out vec2 v_TexCoords;

void main()
{
	gl_Position = u_MVP * vec4(a_Position, 1.0f);
	v_TexCoords = a_TexCoords;
}

#Shader Fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoords;

void main()
{
	vec2 uv = v_TexCoords / vec2(0.1f, 0.1f); // 0->10

	uv = fract(uv);

	float value = step(uv.x, 0.9) * step(uv.y, 0.9);
	value = 1.0 - value;

	vec3 color3 = vec3(0.2f, 0.2f, 0.2f);

	color = vec4(color3 * value, value);
}