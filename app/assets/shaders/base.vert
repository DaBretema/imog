#version 450 core

layout(location = 0) in vec3 Pos;
layout(location = 1) in vec3 Norm;
layout(location = 2) in vec2 TexUV;

out vec3 v_Pos;
out vec3 v_Norm;
out vec2 v_TexUV;

uniform mat4 u_matMV;
uniform mat4 u_matV;
uniform mat4 u_matN;

void main() {
	v_Norm = (u_matN * vec4(Norm,0)).xyz;
	v_Pos = (u_matMV * vec4(Pos,1)).xyz;
	v_TexUV = TexUV;
	gl_Position = u_matMV * vec4(Pos, 1);
}
