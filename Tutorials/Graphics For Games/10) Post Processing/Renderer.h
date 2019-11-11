#pragma once
 #include "../../nclgl/OGLRenderer.h"
 #include "../../nclgl/HeightMap.h"
 #include "../../nclgl/Camera.h"

 #define POST_PASSES 10

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void PresentScene();
	void DrawPostProcess();
	void DrawScene();

	Mesh* quad;
	HeightMap* heightMap;
	Shader* sceneShader, * processShader;
	Camera* camera;
	GLuint bufferFBO, processFBO, bufferColourTex[2], bufferDepthTex;
};
