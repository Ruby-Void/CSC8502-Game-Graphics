#include "PointLights.h"

PointLights::PointLights(int lightNum, int rawWidth, int rawHeight,
						 int heightmapX, int heightmapZ) : 
	lightNum(lightNum), rawWidth(rawWidth), rawHeight(rawHeight),
	heightmapX(heightmapX), heightmapZ(heightmapZ) {

	pointLights = new Light[lightNum * lightNum];
	for (int x = 0; x < lightNum; ++x) {
		for (int z = 0; z < lightNum; ++z) {
			Light& l = pointLights[(x * lightNum) + z];

			float xPos = (rawWidth * heightmapX / (lightNum - 1)) * x, zPos = (rawHeight * heightmapZ / (lightNum - 1)) * z;
			l.SetPosition(Vector3(xPos, 100.0f, zPos));

			float r = 0.5f + (float)(rand() % 129) / 128.0f, 
				  g = 0.5f + (float)(rand() % 129) / 128.0f, 
				  b = 0.5f + (float)(rand() % 129) / 128.0f;
			l.SetColour(Vector4(r, g, b, 1.0f));

			float radius = (rawWidth * heightmapX / lightNum);
			l.SetRadius(radius);
		}
	}

	pointlightShader = new Shader(SHADERDIR"PointLightShaders/PointLightVertex.glsl", SHADERDIR"PointLightShaders/PointLightFragment.glsl");
	pointLightsInit = (!pointlightShader->LinkProgram()) ? false : true;
}

PointLights::~PointLights(void) {
	delete[] pointLights;
	delete pointlightShader;
}
