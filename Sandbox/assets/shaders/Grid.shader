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

uniform float u_Scale;
uniform float u_Resolution;

in vec2 v_TexCoords;

void main()
{	
/*	vec2 uv = v_TexCoords / vec2(u_Scale, u_Scale); // 0->10

	uv = fract(uv);

	float value = step(uv.x, u_Resolution) * step(uv.y, u_Resolution);
	value = 1.0 - value;

	vec3 color3 = vec3(0.2f, 0.2f, 0.2f);

	color = vec4(color3 * value, value); */

	//scale * resolution = number of sqaures
	float scale = u_Scale;
	float resolution = u_Resolution;

	vec2 uv = v_TexCoords * scale;

	vec2 fract = fract(uv * resolution);
	float value = step(resolution, fract.x) * step(resolution, fract.y);

	value = 1.0f - value;

	color = vec4(vec3(0.2f, 0.2f, 0.2f) * value, value);
}