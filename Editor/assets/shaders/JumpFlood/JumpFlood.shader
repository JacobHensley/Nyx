#Shader Vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

layout(location = 0) out vec2 v_TexCoords;
layout(location = 2) out vec2 v_TexelSize;
layout(location = 3) out vec2 v_UV[9];

layout(location = 0) uniform vec2 u_TexelSize;
layout(location = 1) uniform int u_Step;

void main()
{
	gl_Position = vec4(a_Position, 1.0f);
	v_TexCoords = a_TexCoords;

    vec2 UV[9];

    UV[0] = v_TexCoords;

    vec2 dx = vec2(u_TexelSize.x, 0) * u_Step;
    vec2 dy = vec2(0, u_TexelSize.y) * u_Step;

    //Sample all pixels within a 3x3 block
    UV[1] = v_TexCoords + dx;
    UV[2] = v_TexCoords - dx;
    UV[3] = v_TexCoords + dy;
    UV[4] = v_TexCoords - dy;
    UV[5] = v_TexCoords + dx + dy;
    UV[6] = v_TexCoords + dx - dy;
    UV[7] = v_TexCoords - dx + dy;
    UV[8] = v_TexCoords - dx - dy;

    v_UV = UV;
    v_TexelSize = u_TexelSize;
}

#Shader Fragment
#version 450 core

layout(location = 0) in vec2 v_TexCoords;
layout(location = 2) in vec2 v_TexelSize;
layout(location = 3) in vec2 v_UV[9];

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D r_InputTexture;

float ScreenDist(vec2 v) {
    float ratio = v_TexelSize.x / v_TexelSize.y;
    v.x /= ratio;
    return dot(v, v);
}

void checkBounds(inout vec2 xy, vec2 uv) {
    if (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1) {
        xy = vec2(1000, 1000);
    }
}

void main()
{
    vec4 curr = texture(r_InputTexture, v_UV[0]);

    for (uint j = 1; j <= 8; j++) {
        //Sample a neighbor pixel
        vec4 n = texture(r_InputTexture, v_UV[j]);

        //If the neighbor sample is on the other side
        //compared to us, treat it like there is zero
        //distance to the surface
        if (n.w != curr.w) {
            n.xyz = vec3(0, 0, 0);
        }

        //Calculate the delta vector from our curr point
        //to the point the neighbor is pointing to
        n.xy += v_UV[j] - v_UV[0];

        //If the neighbor is out of bounds, make it invalid
        checkBounds(n.xy, v_UV[j]);

        //Calculate the screen space distance to the 
        //neighbors point using this delta
        float dist = ScreenDist(n.xy);

        //If the screen space distance is less than our
        //current min distance, update the current value
        //to point to the new location with the new distance
        if (dist < curr.z) {
            curr.xyz = vec3(n.xy, dist);
        }
    }

    color = curr;
}