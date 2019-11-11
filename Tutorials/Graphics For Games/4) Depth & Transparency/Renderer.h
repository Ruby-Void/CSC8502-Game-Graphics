#pragma once
#include "../../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();

	void ToggleObject();
	void ToggleDepth();
	void ToggleAlphaBlend();
	void ToggleBlendMode();
	void MoveObject(float by);

protected:
	Mesh* meshes[2];
	Vector3 positions[2];
	GLuint textures[2];

	Matrix4 textureMatrix, modelMatrix, projMatrix, viewMatrix;

	int blendMode;
	bool modifyObject, usingDepth, usingAlpha;
};