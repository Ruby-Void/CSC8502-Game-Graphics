#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour[2];

void main(void) {
	vec3 BN = normalize(cross(normalize(IN.normal), normalize(IN.tangent)));
	mat3 TBN = mat3(normalize(IN.tangent), -BN, normalize(IN.normal));
	vec3 normal = normalize(TBN * (texture2D (bumpTex, IN.texCoord).rgb) * 2.0 - 1.0);

	fragColour[0] = texture2D (diffuseTex, IN.texCoord);
	fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
}