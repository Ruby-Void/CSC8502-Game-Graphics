#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/OBJmesh.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/heightmap.h"
#include "../../nclgl/Frustum.h"
#include <algorithm>

#include "Skybox.h"
#include "PointLights.h"

#define LIGHTNUM 8
#define SHADOWSIZE 2048

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	// Scene Handling 
	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	// Scene Dynamics Variables
	bool shadowsActive = true, waterActive = true, 
		pointLightsActive = true, skyboxActive = true,
		lightMovementActive = true;
	float waterRotation, lightRotation;
	Vector3 envIllumPos, centrePos;

	// Scene Variables
	Camera* camera;		// Camera
	Mesh* viewport;		// Viewport
	Mesh* heightMap;	// Terrain
	Mesh* cube;			// Cube
	Mesh* water;		// Water
	OBJMesh* sphere;	// Light Sphere
	Light* envIllum;	// Enviroment Illumination

	// Shaders
	Shader* sceneShader, * sceneShadowShader, * shadowShader, * waterShader, * combinedShader;

	// Frame Buffers
	GLuint envFBO, shadowFBO, skyboxFBO, pointLightFBO;

	// Screen Textures
	GLuint envColourST, envNormalST, envDepthST, 
		   shadowST, skyboxST, emissiveST, specularST;

	// Buffer Clearing
	void ClearAllBuffers();

	// Creation
	Skybox* skybox;				// Skybox Creation
	PointLights* pointLights;	// Point Light Creation
	bool createHeightMap();		// Terrain creation
	bool createCube();			// Cube creation
	bool createWater();			// Water Creation
	bool createSphere();		// Light Sphere Creation
	bool createShaders();		// Shaders Creation
	bool createFBOs();			// Frame Buffers Creation

	// Binding
	void bindEnvBuffer();			// Bind Scene to Enviroment Buffer
	void bindSkyboxBuffer();		// Bind Skybox to Skybox Buffer
	void bindPointLightsBuffer();	// Bind Point Lights to Point Light Buffer
	void bindShadowBuffer();		// Bind shadows to Shadow Buffer


	// Display Different Scenes	
	void displayShadow() { shadowsActive = !shadowsActive; }	
	void displayWater() { waterActive = !waterActive; }
	void displayPointLights() { pointLightsActive = !pointLightsActive; }
	void displaySkybox() { skyboxActive = !skyboxActive; }
	void displayLightMovement() { lightMovementActive = !lightMovementActive; }
	void combineBuffers();

	// Scene Node
	SceneNode* root = nullptr;			// Scene Node Root
	Frustum frameFrustum;				// Frame Frustum
	vector<SceneNode*> transparentNL;	// Transparent Node List
	vector<SceneNode*> opaqueNL;		// Opaque Node List
	void buildNL(SceneNode* from);		// Build Node Lists
	void sortNL();						// Sort Node Lists
	void clearNL();						// Clear Node Lists
	void drawNodes();					// Draw Nodes
	void drawNode(SceneNode* n);		// Draw Node	

private:
	// Generic Functions	
	void GenerateST(GLuint& into, bool depth = false);
	void GenerateShadowST(GLuint& into);
};