#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER, 
	TEXTURE_BUFFER, NORMAL_BUFFER, 
	TANGENT_BUFFER, INDEX_BUFFER, 
	MAX_BUFFER
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

	void SetBumpMap(GLuint tex) { bumpTexture = tex; }
	GLuint GetBumpMap() { return bumpTexture; }

protected:
	unsigned int* indices;

	GLuint arrayObject, bufferObject[MAX_BUFFER], 
		   numVertices, type, texture, 
		   numIndices, bumpTexture;

	Vector2* textureCoords;
	Vector3* vertices, * normals, * tangents;
	Vector4* colours;

	void GenerateNormals();
	void GenerateTangents();
	Vector3 GenerateTangent(const Vector3& a, const Vector3& b, const Vector3& c, const Vector2& ta, const Vector2& tb, const Vector2& tc);

	void BufferData();
};