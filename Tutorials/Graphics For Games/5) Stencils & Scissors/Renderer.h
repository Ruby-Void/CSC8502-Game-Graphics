#pragma once
#include "../../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();

	void ToggleScissor() { usingScissor = !usingScissor; }
	void ToggleStencil() { usingStencil = !usingStencil; }

protected:
	Mesh* triangle, * quad;
	bool usingScissor = false, usingStencil = false;
};