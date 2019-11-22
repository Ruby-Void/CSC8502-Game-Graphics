#pragma once
#include "../../nclgl/OGLRenderer.h"

class PointLights {
public:
	PointLights(int lightNum, int rawWidth, int rawHeight, int heightmapX, int heightmapZ);
	~PointLights(void);

	bool getPointLightsInit() const { return pointLightsInit; }
	Light* getPointLights() const { return pointLights; }
	Shader* getPointLightsShader() const { return pointlightShader; }

protected:
	bool pointLightsInit = false;
	int lightNum, rawWidth, rawHeight, heightmapX, heightmapZ;
	Light* pointLights = nullptr;
	Shader* pointlightShader = nullptr;
};