#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include <algorithm>

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

protected:
	SceneNode* root;
	Camera* camera;
	Frustum frameFrustum;
	vector<SceneNode*> transparentNodeList, nodeList;

	void DrawNodes();
	void DrawNode(SceneNode* n);
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
};