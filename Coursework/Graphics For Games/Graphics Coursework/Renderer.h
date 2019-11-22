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
	float waterRotation;	// Water Rotation
	float lightRotation;	// Light Rotation

	// Scene Variables
	Camera* camera;					// Camera
	Mesh* viewport;					// Viewport
	Mesh* heightMap;				// Terrain
	Mesh* water;					// Water
	OBJMesh* sphere;				// Light Sphere
	Light* envIllum;				// Enviroment Illumination
	Light* waterIllum;				// Water Light
	Shader* sceneShader;			// Scene Shader	
	Shader* shadowShader;			// Shadow Shader
	Shader* combineShadowShader;	// Shadow Scene Shader
	Shader* waterShader;			// Water Shader
	Shader* combinedShader;			// Combined Buffers Shader

	// Frame Buffers
	GLuint envFBO;			// Enviroment Frame Buffer	
	GLuint shadowFBO;		// Shadow Frame Buffer
	GLuint skyboxFBO;		// Skybox Frame Buffer
	GLuint pointLightFBO;	// Point Lights Frame Buffer
	GLuint testFBO;			// Test Frame Buffer

	// Screen Textures
	GLuint envColourST;		// Enviroment Colour Screen Texture
	GLuint envNormalST;		// Enviroment Normal Screen Texture
	GLuint envDepthST;		// Enviroment Depth Screen Texture
	GLuint shadowST;		// Shadow Screen Texture
	GLuint skyboxST;		// Skybox Screen Texture
	GLuint emissiveST;		// Light Emissive Screen Texture
	GLuint specularST;		// Light Specular Screen Texture
	GLuint testST;			// Test Screen Texture

	// Creation
	Skybox* skybox;				// Skybox Creation
	PointLights* pointLights;	// Point Light Creation
	bool createHeightMap();		// Terrain creation
	bool createWater();			// Water Creation
	bool createSphere();		// Light Sphere Creation
	bool createShaders();		// Shaders Creation
	bool createFBOs();			// Frame Buffers Creation

	// Binding
	void bindEnvBuffer();			// Bind Scene to Enviroment Buffer
	void bindSkyboxBuffer();		// Bind Skybox to Skybox Buffer
	void bindPointLightsBuffer();	// Bind Point Lights to Point Light Buffer
	void bindShadowBuffer();		// Bind shadows to Shadow Buffer
	void combineShadowBuffer();
	void combineBuffers();			// Combine All Bound Buffers

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