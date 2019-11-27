#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec3 colour;
in vec2 texCoord;

out Vertex{
	vec2 texCoord;
} OUT;

void main(void) {
	mat4 mvp = modelMatrix * viewMatrix * projMatrix;
	gl_Position = mvp * vec4(position, 1.0);
	OUT.texCoord = texCoord;
}