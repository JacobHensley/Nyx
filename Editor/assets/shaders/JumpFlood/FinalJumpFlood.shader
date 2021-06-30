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

layout(binding = 0) uniform sampler2D r_InputTexture;

void main()
{
    vec4 curr = texture(r_InputTexture, v_TexCoords);

    //Grab the signed distance out of the result
    //Squared distance is stored in the z component
    float dist = sqrt(curr.z);

    float alpha = 0.0f;
    if (dist < 0.003f) // outline thickness
        alpha = 1.0f;
    else
        alpha = 0.0f;
    
    vec3 color;

    //Calculate color based on inside/outside flag
    //w is non-zero if we are inside
    if (curr.w == 0)
        color = vec3(1.0f, 0.4f, 0.0f);

    outColor = vec4(color, alpha);
}