#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 v_pos[];
in vec3 v_norm[];
in vec2 v_texUV[];

out vec3 g_pos;
out vec3 g_norm;
out vec2 g_texUV;

uniform mat4 u_matP;

// Compute normal here at geometry shader
vec3 norm() {
    vec4 BA = gl_in[1].gl_Position - gl_in[0].gl_Position;
    vec4 CA = gl_in[2].gl_Position - gl_in[0].gl_Position;
    return normalize(cross(-BA.xyz, CA.xyz));
}

void main() {

    // Create a "triangle_strip" primitive
    for (int i = 0; i < 3; ++i) {
        g_pos = v_pos[i];
        g_norm = v_norm[i];
        g_texUV = v_texUV[i];
        gl_Position = u_matP * gl_in[i].gl_Position;
        EmitVertex();
    }

    // Close "triangle_strip" primitive
	EndPrimitive();
}
