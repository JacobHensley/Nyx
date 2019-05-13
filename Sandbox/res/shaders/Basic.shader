#Shader Vertex
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

void main() {
	gl_Position.xyz = vertexPosition_modelspace;
	gl_Position.w = 1.0;

}


#Shader Fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main() {
	color = u_Color; //vec4(col.r, col.g, col.b, 1);
}