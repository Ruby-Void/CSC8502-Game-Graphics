#pragma once
#include "../../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();

	void ToggleScissor();
	void ToggleStencil();

protected:
	Mesh* triangle, * quad;
	bool usingScissor = false, usingStencil = false;
};