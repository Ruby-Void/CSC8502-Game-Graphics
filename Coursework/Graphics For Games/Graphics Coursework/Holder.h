#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/OBJmesh.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/heightmap.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
#include "../../nclgl/SceneNode.h"
#include <algorithm>

#define LIGHTNUM 8

class Holder : public OGLRenderer {
public:
	Holder(Window& parent);
	virtual ~Holder(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	float rotation;

	Camera* camera;

	SceneNode* root;

	Mesh* quad, * heightMap;
	OBJMesh* sphere;

	Light* light;
	Light* pointLights;

	GLuint bufferFBO, shadowFBO, pointLightFBO;

	GLuint bufferColourTex, bufferNormalTex, bufferDepthTex, lightEmissiveTex, lightSpecularTex, shadowTex;

	Shader* sceneShader, * shadowShader, * pointlightShader, * combineShader;

	GLuint cubeMapTex;
	GLuint skyboxFBO;
	Shader* skyboxShader;

	bool createSphere();
	bool CreateShaders();
	bool CreateSkybox();
	bool CreateFrameBuffers();
	void CreateHeightMap();
	void CreatePointLights();

	void BindSkyboxBuffer();
	void BindEnviromentBuffer();
	void BindPointLightsBuffer();

	void CombineBuffers();

	void GenerateScreenTexture(GLuint& into, bool depth = false);
};