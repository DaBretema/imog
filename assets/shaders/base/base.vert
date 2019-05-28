#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texUV;

out vec3 v_pos;
out vec3 v_norm;
out vec2 v_texUV;

uniform mat4 u_matMV;
uniform mat4 u_matN;
uniform mat4 u_matV;

void main() {
	v_norm = (u_matN * vec4(norm,0)).xyz;
	v_pos = (u_matMV * vec4(pos,1)).xyz;
	v_texUV = texUV;
	gl_Position = u_matMV * vec4(pos, 1);
}
