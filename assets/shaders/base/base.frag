#version 330 core

in vec3 g_pos;
in vec3 g_norm;
in vec2 g_texUV;

layout(location = 0) out vec4 f_color;

// Uploaded by the renderer
uniform vec3 u_color;
uniform sampler2D u_texture;

// Uploaded by the engine
uniform mat4 u_matMV;
uniform vec3 u_clearColor;

// ====================================================================== //
// ====================================================================== //
// Color with fog
// ====================================================================== //

vec3 fogged(in vec3 pos, in vec3 color){
	float fog_factor = 1 / exp(0.0003 * pow(length(pos),2));
	return mix(u_clearColor, color, fog_factor);
}

// ====================================================================== //
// ====================================================================== //
// Entry point
// ====================================================================== //

void main() {

  // Obj color and texture read
	vec3 color = texture(u_texture, g_texUV).rgb;
	if (color == vec3(0)) { color = u_color; }

  color *= u_color * 0.5;
  color = fogged(-g_pos*0.25, color);

  // Output gamma corrected color
  f_color = vec4(color, 1.0);
}
