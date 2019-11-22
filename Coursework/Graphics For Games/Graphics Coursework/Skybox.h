#pragma once
#include "../../nclgl/OGLRenderer.h"

class Skybox {
public:
	Skybox(void);
	~Skybox(void);

	bool getSkyboxInit() const { return skyboxInit; }
	GLuint getSkyboxCubeMap() const { return skyboxCubeMap; }
	Shader* getSkyboxShader() const { return skyboxShader; }

protected:
	bool skyboxInit = false;
	GLuint skyboxCubeMap;
	Shader* skyboxShader = nullptr;
};