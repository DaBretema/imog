#version 450 core

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

// Should be uniforms
const float shininess = 128.0;



// ====================================================================== //
// ====================================================================== //
// Gamma correction
// ====================================================================== //

vec3 gammed(in vec3 color){
  float sRGB = 2.2;
  return pow(color, vec3(1.0 / sRGB));
}


// ====================================================================== //
// ====================================================================== //
// Color with fog
// ====================================================================== //

vec3 fogged(in vec3 pos, in vec3 color){
	float fog_factor = 1 / exp(0.01 * pow(length(pos),2));
	return mix(u_clearColor, color, fog_factor);
}


// ====================================================================== //
// ====================================================================== //
// Toon calc
// ====================================================================== //

vec3 toon(in vec3 color, in float toonFactor) {
	toonFactor += 1;
	return floor(color * toonFactor) / toonFactor;
}


// ====================================================================== //
// ====================================================================== //
// Entry point
// ====================================================================== //

void main() {

  vec3 norm = normalize(g_norm);
  float ambient = clamp(0.25, 0.1, 0.9);
  vec3 lightPos = (u_matMV * vec4(u_lightPos,1)).xyz;

  // Obj color and texture read
	vec3 color = texture(u_texture, g_texUV).rgb;
	if (color == vec3(0)) { color = u_color; }

  // Light
  vec3 lightDir = lightPos - g_pos;
  float lightDist = length(lightDir);
  lightDir = normalize(lightDir);
  vec3 light = u_lightColor * (u_lightIntensity / (lightDist*lightDist));

  // Diffuse
  float diffuse = max(dot(lightDir, norm), ambient);

  // Final color with gamma correction for sRGB
  color *= diffuse * light;

  // Output gamma corrected color
  f_color = vec4(color, 1.0);
}
