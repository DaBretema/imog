#version 450 core

in vec3 g_Pos;
in vec3 g_Norm;
layout(location = 0) out vec4 f_Color;

// Uploaded by the renderer
uniform vec3 Color;

// Uploaded by the engine
uniform mat4 u_matV;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;

// Frag shader globals
vec3 V;
vec3 N;

///
/// \param color, where apply the operation
/// \param toonFactor, must be greater than zero
///
vec3 toon(in vec3 color, in float toonFactor)
{
	toonFactor += 1;
	return floor(color * toonFactor) / toonFactor;
}

///
/// Calc the color for ONE light, using uniforms data
///
vec3 shadeIt()
{
	// Light data
	vec3 L = normalize((vec4(u_lightPos,1)).xyz - g_Pos);
	vec3 H = normalize(L + V); // For blinn phong
	// Object color
	vec3 diffuse = clamp(Color * dot(N, L), 0, 1);
	vec3 specular = vec3(1.) * pow(max(dot(N, H), 0.0001), 5);
	// // Toon that colors
	// diffuse = toon(diffuse, 4);
	// specular = toon(specular, 1);
	return u_lightColor * (diffuse + specular);
}

void main()
{
	V = normalize(-g_Pos);
	N = normalize(g_Norm);

	vec3 temp_Color = Color * vec3(0.5);
	temp_Color += shadeIt();

	f_Color = vec4(temp_Color,1.0);
}
