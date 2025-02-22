#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent), waterRotation(0.0f), lightRotation(0.0f) {
	skybox = new Skybox();
	pointLights = new PointLights(LIGHTNUM, RAW_WIDTH, RAW_HEIGHT, HEIGHTMAP_X, HEIGHTMAP_Z);
	if (!skybox->getSkyboxInit() || !pointLights->getPointLightsInit() || createObjectTextures() || 
		createHeightMap() || createCube() || createWater() || 
		createSphere() || createShaders() || createFBOs()) {
		return;
	}
	
	camera = new Camera(-30.0f, 180.0f, 0.0f, Vector3(2060.0f, 1000.0f, -500.0f));
	viewport = Mesh::GenerateQuad();	
	centrePos = Vector3((RAW_WIDTH * HEIGHTMAP_X) / 2.0f, 400.0f, (RAW_HEIGHT * HEIGHTMAP_Z) / 2.0f);
	envIllumPos = Vector3(-(RAW_WIDTH * HEIGHTMAP_X) / 2.5f, 2000.0f, (RAW_HEIGHT * HEIGHTMAP_Z) / 2.5f);
	envIllum = new Light(envIllumPos, Vector4(0.9f, 0.9f, 1.0f, 1), 100000.0f);
	
	root = new SceneNode();

	SceneNode* terrain = new SceneNode(heightMap);

	SceneNode* cubeOne = new SceneNode(cube);
	cubeOne->SetModelScale(Vector3(2.5f, 2.5f, 2.5f));
	cubeOne->SetTransform(Matrix4::Translation(Vector3(200.0f, 325.0f, 750.0f)));
	terrain->AddChild(cubeOne);
	
	SceneNode* cubeTwo = new SceneNode(cube);
	cubeTwo->SetModelScale(Vector3(2.5f, 2.5f, 2.5f));
	cubeTwo->SetTransform(Matrix4::Translation(Vector3(1050.0f, 325.0f, 1400.0f)));
	terrain->AddChild(cubeTwo);

	SceneNode* cubeThree = new SceneNode(cube);
	cubeThree->SetModelScale(Vector3(3.5f, 3.5f, 3.5f));
	cubeThree->SetTransform(Matrix4::Translation(Vector3(2400.0f, 325.0f, 1750.0f)));
	terrain->AddChild(cubeThree);
	
	root->AddChild(terrain);	

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}

Renderer::~Renderer(void) {
	delete camera;				
	delete viewport;			
	delete heightMap;			
	delete water;				
	delete sphere;		
	delete envIllum;
	delete sceneShader;
	delete sceneShadowShader;	
	delete shadowShader;		
	delete waterShader;			
	delete combinedShader;	
	delete fxaaShader;
	delete skybox;				
	delete pointLights;			
	delete root;				

	glDeleteTextures(1, &envColourST);	
	glDeleteTextures(1, &envNormalST);
	glDeleteTextures(1, &envDepthST);
	glDeleteTextures(1, &shadowST);
	glDeleteTextures(1, &skyboxST);
	glDeleteTextures(1, &emissiveST);
	glDeleteTextures(1, &specularST);
	glDeleteTextures(1, &combineST);

	glDeleteFramebuffers(1, &envFBO);
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteFramebuffers(1, &skyboxFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	glDeleteFramebuffers(1, &combinedFBO);

	currentShader = nullptr;
}

// Scene Handling 
void Renderer::RenderScene() {
	buildNL(root);
	sortNL();
	ClearAllBuffers();
	if (shadowsActive) bindShadowBuffer();
	if (skyboxActive) bindSkyboxBuffer();	
	bindEnvBuffer();	
	if (pointLightsActive) bindPointLightsBuffer();	
	bindCombineBuffer();
	fxaaProcess();
	SwapBuffers();
	clearNL();
}

void Renderer::UpdateScene(float msec) {	
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F)) { 
		camera->SetFreeCamera(!camera->GetFreeCamera());
		if (!camera->GetFreeCamera()) {			
			camera->SetYaw(180.0f);
			camera->SetPitch(-30.0f);
			camera->SetPosition(Vector3(2060.0f, 1000.0f, -500.0f));
		}		
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) { displayShadow(); }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) { displayWater(); }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) { displayPointLights(); }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4)) { displaySkybox(); }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_5)) { displayLightMovement(); }
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_6)) { 
		lightMovementActive = false;
		camera->SetFreeCamera(true);
		camera->SetPosition(envIllum->GetPosition()); 
	}

	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	root->Update(msec);
	waterRotation += msec / 2000.0f;
	lightRotation = msec * 0.01f;
	envIllum->SetPosition(lightMovementActive ? Matrix4::Rotation(-0.15f, Vector3(0.0f, 1.0f, 0.0f)) * 
						 (envIllum->GetPosition() - centrePos) + centrePos : envIllum->GetPosition());
}

void Renderer::ClearAllBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, envFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, combinedFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

bool Renderer::createObjectTextures() {
	terrainTexture[0] = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	terrainTexture[1] = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG ", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeTexture[0] = SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeTexture[1] = SOIL_load_OGL_texture(TEXTUREDIR"brickDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	return false;
}

// Creation
bool Renderer::createHeightMap() {
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	heightMap->SetTexture(terrainTexture[0]);
	heightMap->SetBumpMap(terrainTexture[1]);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
	return heightMap->GetTexture() && heightMap->GetBumpMap() ? false : true;
}

bool Renderer::createCube() {
	OBJMesh* m = new OBJMesh();
	m->LoadOBJMesh(MESHDIR"cube.obj");
	cube = m;
	cube->SetTexture(cubeTexture[0]);
	cube->SetBumpMap(cubeTexture[1]);
	SetTextureRepeating(cube->GetTexture(), true);
	SetTextureRepeating(cube->GetBumpMap(), true);
	return cube ? false : true;
}

bool Renderer::createWater() {
	water = Mesh::GenerateQuad();
	water->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(water->GetTexture(), true);
	return water->GetTexture() ? false : true;
}

bool Renderer::createSphere() {
	sphere = new OBJMesh();
	return sphere->LoadOBJMesh(MESHDIR"ico.obj") ? false : true;
}

bool Renderer::createShaders() {
	sceneShader = new Shader(VERTEXDIR"BumpVertex.glsl", FRAGMENTDIR"BufferFragment.glsl");
	sceneShadowShader = new Shader(VERTEXDIR"ShadowSceneVertex.glsl", FRAGMENTDIR"ShadowSceneFragment.glsl");
	shadowShader = new Shader(VERTEXDIR"ShadowVertex.glsl", FRAGMENTDIR"ShadowFragment.glsl");
	waterShader = new Shader(VERTEXDIR"PerPixelVertex.glsl", FRAGMENTDIR"ReflectFragment.glsl");
	combinedShader = new Shader(VERTEXDIR"CombineVertex.glsl", FRAGMENTDIR"CombineFragment.glsl");
	fxaaShader = new Shader(VERTEXDIR"FXAA-Vertex.glsl", FRAGMENTDIR"FXAA-Fragment.glsl");
	return (sceneShader->LinkProgram() && sceneShadowShader->LinkProgram() && shadowShader->LinkProgram() && 
		waterShader->LinkProgram() && combinedShader->LinkProgram() && fxaaShader->LinkProgram()) ? false : true;
}

bool Renderer::createFBOs() {
	glGenFramebuffers(1, &envFBO);
	glGenFramebuffers(1, &shadowFBO);
	glGenFramebuffers(1, &skyboxFBO);
	glGenFramebuffers(1, &pointLightFBO);
	glGenFramebuffers(1, &combinedFBO);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	GenerateST(envColourST);
	GenerateST(envNormalST);
	GenerateST(envDepthST, true);	
	GenerateST(skyboxST);
	GenerateST(emissiveST);
	GenerateST(specularST);
	GenerateST(combineST);
	GenerateShadowST(shadowST);

	glBindFramebuffer(GL_FRAMEBUFFER, envFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, envColourST, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, envNormalST, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, envDepthST, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { return true; }

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowST, 0);
	glDrawBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { return true; }

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, skyboxST, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { return true; }

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, emissiveST, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, specularST, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { return true; }

	glBindFramebuffer(GL_FRAMEBUFFER, combinedFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, combineST, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { return true; }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return false;
}

// Binding
void Renderer::bindShadowBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	SetCurrentShader(shadowShader);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	viewMatrix = Matrix4::BuildViewMatrix(envIllum->GetPosition(), centrePos);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	shadowMatrix = biasMatrix * (projMatrix * viewMatrix);

	UpdateShaderMatrices();
	drawNodes();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Renderer::bindSkyboxBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glDepthMask(GL_FALSE);

	SetCurrentShader(skybox->getSkyboxShader());

	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	UpdateShaderMatrices();
	viewport->Draw();

	glDepthMask(GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Renderer::bindEnvBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, envFBO);
	glEnable(GL_CULL_FACE);
	if (shadowsActive) {
		SetCurrentShader(sceneShadowShader);
		SetShaderLight(*envIllum);

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
		
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, shadowST);

		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

		UpdateShaderMatrices();
		drawNodes(true);
	}
	else {
		SetCurrentShader(sceneShader);

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);

		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
		
		UpdateShaderMatrices();
		drawNodes();
	}
	
	if (waterActive) {
		SetCurrentShader(waterShader);
		SetShaderLight(*envIllum);

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxActive ? skybox->getSkyboxCubeMap() : 0);

		float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f), heightY = 256 * HEIGHTMAP_Y / 3.0f, heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);
		modelMatrix = Matrix4::Translation(Vector3(heightX, heightY, heightZ)) * Matrix4::Scale(Vector3(heightX, 1, heightZ)) * Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
		textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(waterRotation, Vector3(0.0f, 0.0f, 10.0f));

		UpdateShaderMatrices();
		water->Draw();
	}	

	glCullFace(GL_BACK);

	textureMatrix.ToIdentity();

	glDisable(GL_CULL_FACE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Renderer::bindPointLightsBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glEnable(GL_CULL_FACE);
	SetCurrentShader(pointLights->getPointLightsShader());

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "normTex"), 4);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, envDepthST);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, envNormalST);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), (1.0f / width), (1.0f / height));

	Vector3 translate = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));
	Matrix4 pushMatrix = Matrix4::Translation(translate), popMatrix = Matrix4::Translation(-translate);

	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light& l = pointLights->getPointLights()[(x * LIGHTNUM) + z];
			float radius = l.GetRadius();

			modelMatrix = pushMatrix * Matrix4::Rotation(lightRotation, Vector3(0, 1, 0)) *
						  popMatrix * Matrix4::Translation(l.GetPosition()) *
						  Matrix4::Scale(Vector3(radius * 1.1f, radius * 1.1f, radius * 1.1f));

			l.SetPosition(modelMatrix.GetPositionVector());

			SetShaderLight(l);
			UpdateShaderMatrices();
			
			float dist = (l.GetPosition() - camera->GetPosition()).Length();
			glCullFace(dist < radius ? GL_FRONT : GL_BACK);
			sphere->Draw();
		}
	}

	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Renderer::bindCombineBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, combinedFBO);
	SetCurrentShader(combinedShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);	
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "emissiveTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "specularTex"), 4);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 5);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "skyboxTex"), 6);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, envColourST);

	if (pointLightsActive) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, emissiveST);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, specularST);
	}	

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, envDepthST);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, skyboxST);

	viewMatrix = camera->BuildViewMatrix();
	modelMatrix.ToIdentity();

	UpdateShaderMatrices();
	viewport->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}



void Renderer::fxaaProcess() {
	SetCurrentShader(fxaaShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "combinedTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, combineST);

	Vector2 screenDimension(width, height);
	glUniform2fv(glGetUniformLocation(currentShader->GetProgram(), "frameBufSize"), 1, (float*)&screenDimension);

	viewMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);

	UpdateShaderMatrices();
	viewport->Draw();

	glUseProgram(0);
}

// Scene Node
void Renderer::buildNL(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNL.push_back(from);
		}
		else {
			opaqueNL.push_back(from);
		}
	}

	for (vector <SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		buildNL((*i));
	}
}

void Renderer::sortNL() {
	std::sort(transparentNL.begin(), transparentNL.end(), SceneNode::CompareByCameraDistance);
	std::sort(opaqueNL.begin(), opaqueNL.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::clearNL() {
	transparentNL.clear();
	opaqueNL.clear();
}

void Renderer::drawNodes(bool shadows) {
	for (vector<SceneNode*>::const_iterator i = opaqueNL.begin(); i != opaqueNL.end(); ++i) {
		drawNode((*i), shadows);
	}

	
	for (vector<SceneNode*>::const_reverse_iterator i = transparentNL.rbegin(); i != transparentNL.rend(); ++i) {
		drawNode((*i), shadows);
	}	
}

void Renderer::drawNode(SceneNode* n, bool shadows) {
	if (n->GetMesh()) {
		if (shadows) {
			Matrix4 tempMatrix = shadowMatrix * modelMatrix * n->GetWorldTransform()* Matrix4::Scale(n->GetModelScale());

			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false, *&tempMatrix.values);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(modelMatrix * n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale())));
			glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());
		}
		else {
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale())));
			glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());
		}
		n->Draw();
	}
}

// Generic Functions
void Renderer::GenerateST(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8, width, height, 0, depth ? GL_DEPTH_COMPONENT : GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::GenerateShadowST(GLuint& into) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);
}