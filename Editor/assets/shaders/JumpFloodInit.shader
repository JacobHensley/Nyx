#Shader Vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

layout(location = 2) out vec2 v_TexCoords;

void main()
{
	gl_Position = vec4(a_Position, 1.0f);
	v_TexCoords = a_TexCoords;
}

#Shader Fragment
#version 450 core

layout(location = 2) in vec2 v_TexCoords;

layout(location = 0) out vec4 outColor;

layout(location = 4) uniform vec2 u_TextureSize;
layout(location = 5) uniform sampler2D r_InputTexture;

float ScreenDist(vec2 v) {
    float ratio = u_TextureSize.x / u_TextureSize.y; // texture size, texel size
    v.x /= ratio;
    return dot(v, v);
}

void main()
{
    vec4 color = texture(r_InputTexture, v_TexCoords);

    vec4 result;
    result.xy = vec2(100, 100);
    result.z = ScreenDist(result.xy);

    //This line determines what is considered inside vs outside
    //For this example, alpha is the determining factor
    result.w = color.a > 0.5 ? 1 : 0;

    //All pixels start out as pointing wayy too far out.
    //100 image units is very far.
    //W flags whether or not we are inside or outside

    outColor = result;
}