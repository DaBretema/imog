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
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform float u_lightIntensity;
uniform vec3 u_clearColor;


void main() {

  // Texture
	vec3 color = texture(u_texture, g_texUV).rgb;
	if (color == vec3(0)) { color = u_color; }

  // Light
  vec3 norm = normalize(g_norm);
  vec3 viewDir = normalize(-g_pos);
  color *= max(dot(viewDir, norm), 0.1);

  // Aesthetic
  vec3 cMod = vec3(0.2);
  color = clamp(color, u_color-cMod, u_color+cMod);

  // Output gamma corrected color
  f_color = vec4(color, 1);
}
