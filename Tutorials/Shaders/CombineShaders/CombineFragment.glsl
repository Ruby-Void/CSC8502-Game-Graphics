#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D emissiveTex;
uniform sampler2D specularTex;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {
	vec3 diffuse = texture (diffuseTex, IN.texCoord).xyz;
	vec3 light = texture (emissiveTex, IN.texCoord).xyz;
	vec3 specular = texture (specularTex, IN.texCoord).xyz;

	fragColour.xyz = diffuse.xyz * 0.2;
	fragColour.xyz += diffuse.xyz * light;
	fragColour.xyz += specular;
	fragColour.a = 1.0;}