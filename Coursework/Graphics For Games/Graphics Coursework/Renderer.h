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
	Camera* camera;		
	Mesh* viewport;		
	Mesh* heightMap;	
	Mesh* cube;			
	Mesh* water;		
	OBJMesh* sphere;	
	Light* envIllum;	

	// Object Textures
	GLuint terrainTexture[2], cubeTexture[2];

	// Shaders
	Shader* sceneShader, * sceneShadowShader, * shadowShader, 
		  * waterShader, * combinedShader, * fxaaShader;

	// Frame Buffers
	GLuint envFBO, shadowFBO, skyboxFBO,
		   pointLightFBO, combinedFBO;

	// Screen Textures
	GLuint envColourST, envNormalST, envDepthST, 
		   shadowST, skyboxST, emissiveST, 
		   specularST, combineST;

	// Buffer Clearing
	void ClearAllBuffers();

	// Creation
	Skybox* skybox;					// Skybox Creation
	PointLights* pointLights;		// Point Light Creation
	bool createObjectTextures();	// Object Texture Creation
	bool createHeightMap();			// Terrain creation
	bool createCube();				// Cube creation
	bool createWater();				// Water Creation
	bool createSphere();			// Light Sphere Creation
	bool createShaders();			// Shaders Creation
	bool createFBOs();				// Frame Buffers Creation

	// Binding
	void bindEnvBuffer();			// Bind Scene to Enviroment Buffer
	void bindSkyboxBuffer();		// Bind Skybox to Skybox Buffer
	void bindPointLightsBuffer();	// Bind Point Lights to Point Light Buffer
	void bindShadowBuffer();		// Bind shadows to Shadow Buffer
	void bindCombineBuffer();		// Bind all buffers to main

	// Display Different Scenes	
	void displayShadow() { shadowsActive = !shadowsActive; }	
	void displayWater() { waterActive = !waterActive; }
	void displayPointLights() { pointLightsActive = !pointLightsActive; }
	void displaySkybox() { skyboxActive = !skyboxActive; }
	void displayLightMovement() { lightMovementActive = !lightMovementActive; }
	
	// Post Processing
	void fxaaProcess();	// FXAA Post Process

	// Scene Node
	SceneNode* root = nullptr;						// Scene Node Root
	Frustum frameFrustum;							// Frame Frustum
	vector<SceneNode*> transparentNL;				// Transparent Node List
	vector<SceneNode*> opaqueNL;					// Opaque Node List
	void buildNL(SceneNode* from);					// Build Node Lists
	void sortNL();									// Sort Node Lists
	void clearNL();									// Clear Node Lists
	void drawNodes(bool shadows = false);			// Draw Nodes
	void drawNode(SceneNode* n, bool shadows);		// Draw Node	

private:
	// Generic Functions	
	void GenerateST(GLuint& into, bool depth = false);
	void GenerateShadowST(GLuint& into);
};