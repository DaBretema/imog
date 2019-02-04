#version 450 core

in vec3 g_Pos;
in vec3 g_Norm;
in vec2 g_TexUV;
layout(location = 0) out vec4 f_Color;


// Uploaded by the renderer
uniform vec3 u_color;
uniform sampler2D u_texture;

// Uploaded by the engine
uniform mat4 u_matV;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;


// ====================================================================== //
// ====================================================================== //
// Frag shader globals
// ====================================================================== //

vec3 V;
vec3 N;
vec3 Kd;


// ====================================================================== //
// ====================================================================== //
// Toon calc
// ====================================================================== //

vec3 toon(in vec3 color, in float toonFactor)
{
	toonFactor += 1;
	return floor(color * toonFactor) / toonFactor;
}

// ====================================================================== //
// ====================================================================== //
// Light compute
// ====================================================================== //

vec3 shade()
{
	// Light data
	vec3 L = normalize((vec4(u_lightPos,1)).xyz - g_Pos);
	vec3 H = normalize(L + V); // For blinn phong
	// Object color
	vec3 diffuse = clamp(Kd * dot(N, L), 0, 1);
	vec3 specular = vec3(1.) * pow(max(dot(N, H), 0.0001), 5);
	// // Toonize that colors
	// diffuse = toon(diffuse, 4);
	// specular = toon(specular, 1);
	return u_lightColor * (diffuse + specular);
}


// ====================================================================== //
// ====================================================================== //
// Entry point
// ====================================================================== //

void main()
{
	V = normalize(-g_Pos);
	N = normalize(g_Norm);

	Kd = texture(u_texture, g_TexUV).rgb * 0.5;
	if (Kd == vec3(0)) { Kd = u_color; }

	f_Color = vec4(Kd + shade(),1.0);
}
