/* Steal code for skybox in projects.*/
/* Steal code for water movement in projects. */
# pragma once
 #include "../../nclgl/OGLRenderer.h"
 #include "../../nclgl/Camera.h"
 #include "../../nclgl/HeightMap.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	float waterRotate;
	GLuint cubeMap;
	Shader* lightShader, * reflectShader, * skyboxShader;
	HeightMap* heightMap;
	Mesh* quad;
	Light* light;
	Camera* camera;

	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
};