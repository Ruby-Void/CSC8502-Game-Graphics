# pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/OBJmesh.h"
#include "../../nclgl/heightmap.h"

#define LIGHTNUM 8

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	float rotation; 
	GLuint bufferFBO, bufferColourTex, bufferNormalTex, bufferDepthTex, pointLightFBO, lightEmissiveTex, lightSpecularTex;
	Shader* sceneShader, * pointlightShader, * combineShader;
	Light* pointLights; 
	Mesh* heightMap; 
	OBJMesh* sphere; 
	Mesh* quad;
	Camera* camera;	

	void FillBuffers();
	void DrawPointLights();
	void CombineBuffers();
	void GenerateScreenTexture(GLuint& into, bool depth = false);
};
