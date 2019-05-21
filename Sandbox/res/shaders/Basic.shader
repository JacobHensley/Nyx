#Shader Vertex
#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 textureCoord;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(pos, 1.0);
	TexCoord = textureCoord;
}
#Shader Fragment

#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
	FragColor = texture(ourTexture, TexCoord);
}