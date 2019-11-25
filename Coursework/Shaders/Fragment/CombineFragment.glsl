#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D emissiveTex;
uniform sampler2D specularTex;
uniform sampler2D depthTex;
uniform sampler2D skyboxTex;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {	
	vec3 diffuse = texture (diffuseTex, IN.texCoord).xyz;
	vec3 emissive = texture (emissiveTex, IN.texCoord).xyz;
	vec3 specular = texture (specularTex, IN.texCoord).xyz;
	vec4 depth = texture (depthTex, IN.texCoord);
	vec4 cube = texture (skyboxTex, IN.texCoord);

	if (depth.r < 1) {
		fragColour.xyz = diffuse.xyz * 0.4;
		fragColour.xyz += diffuse.xyz * (emissive * 1.5);
		fragColour.xyz += specular;
		fragColour.a = 1.0;
	}
	else {		
		fragColour = cube;
	}	}