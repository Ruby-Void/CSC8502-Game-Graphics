#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
		VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, MAX_BUFFER
};

class Mesh {
public:
	Mesh(void);
	~Mesh(void);	

	GLuint GetTexture() { return texture; }
	void SetTexture(GLuint tex) { texture = tex; }

	virtual void Draw();
	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad();

protected:
	GLuint arrayObject, bufferObject[MAX_BUFFER], numVertices, type, texture;

	Vector2* textureCoords;
	Vector3* vertices;
	Vector4* colours;

	void BufferData();
};