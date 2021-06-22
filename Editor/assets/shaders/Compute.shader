#Shader Compute
#version 450 core

layout(binding = 0, rgba16f) restrict writeonly uniform image2D o_Image;

shared vec3 blue;

layout(local_size_x = 32, local_size_y = 32) in;
void main()
{
	if (gl_GlobalInvocationID.x == 0 && gl_GlobalInvocationID.y == 0)
	{
		blue = vec3(0.2, 0.3, 0.8);
	}

	barrier();

	vec4 color = vec4(0, 0, 0, 1);
	color.rg = gl_LocalInvocationID.xy / 32.0f;
	color.b = gl_GlobalInvocationID.x / 1024.0f;
	color.rgb = blue;
	ivec2 invocation = ivec2(gl_GlobalInvocationID);

	color.r = float(invocation.x) / 1024.0;
	color.g = float(invocation.y) / 1024.0;

	float index = float(gl_LocalInvocationIndex) / (32.0f * 32.0f);
	color.rg = vec2(0.0);
	color.b = index;

	imageStore(o_Image, ivec2(gl_GlobalInvocationID), color);
}