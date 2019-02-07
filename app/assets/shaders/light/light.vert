#version 450 core

layout(location = 0) in vec3 pos;
uniform mat4 u_matMVP;

void main() {
	gl_Position = u_matMVP * vec4(pos, 1);
}
