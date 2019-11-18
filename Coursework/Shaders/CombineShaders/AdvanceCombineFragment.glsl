#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D emissiveTex;
uniform sampler2D specularTex;
uniform sampler2D cubeTex;
uniform sampler2D depthBuffer;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {
	vec4 cube = texture (cubeTex, IN.texCoord);
	vec3 diffuse = texture (diffuseTex, IN.texCoord).xyz;
	vec3 light = texture (emissiveTex, IN.texCoord).xyz;
	vec3 specular = texture (specularTex, IN.texCoord).xyz;
	vec4 depth = texture (depthBuffer, IN.texCoord);

	if (depth.r < 1) {
		fragColour.xyz = diffuse.xyz * 0.2;
		fragColour.xyz += diffuse.xyz * light;
		fragColour.xyz += specular;
		fragColour.a = 1.0;
	}
	else {		
		fragColour = cube;
	}	}