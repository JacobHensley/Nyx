#Shader Vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

layout(location = 0) out vec2 v_TexCoords;

void main()
{
	gl_Position = vec4(a_Position, 1.0f);
	v_TexCoords = a_TexCoords;
}

#Shader Fragment
#version 450 core

layout(location = 0) in vec2 v_TexCoords;
layout(location = 0) out vec4 color;

layout(location = 0) uniform float r_Exposure;
layout(location = 1) uniform bool r_ExposureActive;
layout(location = 2) uniform sampler2D r_InputTexture;

void main()
{
	const float gamma = 2.2;
	const float pureWhite = 1.0;

	vec3 textureColor = texture(r_InputTexture, v_TexCoords).rgb * 1.0;
	// Reinhard tonemapping operator.
	// see: "Photographic Tone Reproduction for Digital Images", eq. 4
	float luminance = dot(textureColor, vec3(0.2126, 0.7152, 0.0722));
	float mappedLuminance = (luminance * (1.0 + luminance / (pureWhite * pureWhite))) / (1.0 + luminance);

	// Scale color by ratio of average luminances.
	vec3 mappedColor = (mappedLuminance / luminance) * textureColor;

	// Gamma correction.
	color = vec4(pow(mappedColor, vec3(1.0 / gamma)), 1.0);
	color = vec4(texture(r_InputTexture, v_TexCoords).r, 0.0f, 0.0f, 0.0f);
}