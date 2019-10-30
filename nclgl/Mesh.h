#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
		VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, MAX_BUFFER
};

class Mesh {
public:
	Mesh(void);
	~Mesh(void);

	virtual void Draw();
	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad();

	GLuint GetTexture() { return texture; }
	void SetTexture(GLuint tex) { texture = tex; }

protected:
	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;
	GLuint texture;

	Vector2* textureCoords;
	Vector3* vertices;
	Vector4* colours;

	void BufferData();
};