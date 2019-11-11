#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "Enviroment.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	SceneNode* root;
	
	Camera* camera;

	GLuint cubeMap;
	Shader* skyboxShader;
	Mesh* quad;

	void DrawSkybox();
	void DrawNode(SceneNode* n);
};