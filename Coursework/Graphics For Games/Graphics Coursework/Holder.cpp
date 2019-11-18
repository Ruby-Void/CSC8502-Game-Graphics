#include "Holder.h"

Holder::Holder(Window& parent) : OGLRenderer(parent) {
	camera = new Camera(0.0f, 0.0f, 0.0f, Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_WIDTH * HEIGHTMAP_X));
	quad = Mesh::GenerateQuad();

	if (createSphere() || CreateShaders() || CreateSkybox() || CreateFrameBuffers()) {
		return;
	}
	CreateHeightMap();
	CreatePointLights();

	root->AddChild(new SceneNode(heightMap, sceneShader));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}

Holder::~Holder(void) {
	delete camera;
	delete quad;
	delete heightMap;
	delete sphere;
	delete light;
	delete[] pointLights;
	delete sceneShader;
	delete pointlightShader;
	delete skyboxShader;
	delete combineShader;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &cubeMapTex);
	glDeleteTextures(1, &shadowTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteFramebuffers(1, &skyboxFBO);
	glDeleteFramebuffers(1, &pointLightFBO);

	currentShader = nullptr;
}

void Holder::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindSkyboxBuffer();
	BindEnviromentBuffer();
	BindPointLightsBuffer();
	CombineBuffers();
	SwapBuffers();
}

void Holder::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	rotation = msec * 0.01f;
}

bool Holder::createSphere() {
	sphere = new OBJMesh();
	return (!sphere->LoadOBJMesh(MESHDIR"ico.obj")) ? true : false;
}

bool Holder::CreateShaders() {
	currentShader = new Shader(SHADERDIR"SceneShaders/SceneVertex.glsl", SHADERDIR"SceneShaders/SceneFragment.glsl");
	sceneShader = new Shader(SHADERDIR"BumpShaders/BumpVertex.glsl", SHADERDIR"BufferFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"SkyboxShaders/SkyboxVertex.glsl", SHADERDIR"SkyboxShaders/SkyboxFragment.glsl");
	combineShader = new Shader(SHADERDIR"CombineShaders/AdvanceCombineVertex.glsl", SHADERDIR"CombineShaders/AdvanceCombineFragment.glsl");
	pointlightShader = new Shader(SHADERDIR"PointLightShaders/PointLightVertex.glsl", SHADERDIR"PointLightShaders/PointLightFragment.glsl");
	return (!skyboxShader->LinkProgram() || !currentShader->LinkProgram() || !sceneShader->LinkProgram() ||
		!combineShader->LinkProgram() || !pointlightShader->LinkProgram()) ? true : false;
}

bool Holder::CreateSkybox() {
	cubeMapTex = SOIL_load_OGL_cubemap(TEXTUREDIR"Skybox/purplenebula_ft.tga", TEXTUREDIR"Skybox/purplenebula_bk.tga",
		TEXTUREDIR"Skybox/purplenebula_up.tga", TEXTUREDIR"Skybox/purplenebula_dn.tga",
		TEXTUREDIR"Skybox/purplenebula_rt.tga", TEXTUREDIR"Skybox/purplenebula_lf.tga",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	return (!cubeMapTex) ? true : false;
}

void Holder::CreateHeightMap() {
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG ",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
}

void Holder::CreatePointLights() {
	pointLights = new Light[LIGHTNUM * LIGHTNUM];
	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light& l = pointLights[(x * LIGHTNUM) + z];

			float xPos = (RAW_WIDTH * HEIGHTMAP_X / (LIGHTNUM - 1)) * x;
			float zPos = (RAW_HEIGHT * HEIGHTMAP_Z / (LIGHTNUM - 1)) * z;
			l.SetPosition(Vector3(xPos, 100.0f, zPos));

			float r = 0.5f + (float)(rand() % 129) / 128.0f;
			float g = 0.5f + (float)(rand() % 129) / 128.0f;
			float b = 0.5f + (float)(rand() % 129) / 128.0f;
			l.SetColour(Vector4(r, g, b, 1.0f));

			float radius = (RAW_WIDTH * HEIGHTMAP_X / LIGHTNUM);
			l.SetRadius(radius);
		}
	}
}

bool Holder::CreateFrameBuffers() {
	bool unbound = false;
	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &skyboxFBO);
	glGenFramebuffers(1, &pointLightFBO);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	// Generate our scene depth texture ...
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(shadowTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);
	GenerateScreenTexture(cubeMapTex);

	// And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		unbound = true;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		unbound = true;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cubeMapTex, 0);
	glDrawBuffers(1, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		unbound = true;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		unbound = true;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return unbound;
}

void Holder::BindSkyboxBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, 1, -1);
	UpdateShaderMatrices();
	quad->Draw();
	glDepthMask(GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Holder::BindEnviromentBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();
	heightMap->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Holder::BindPointLightsBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(pointlightShader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "normTex"), 4);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Vector3 translate = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));
	Matrix4 pushMatrix = Matrix4::Translation(translate);
	Matrix4 popMatrix = Matrix4::Translation(-translate);

	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light& l = pointLights[(x * LIGHTNUM) + z];
			float radius = l.GetRadius();

			modelMatrix = pushMatrix * Matrix4::Rotation(rotation, Vector3(0, 1, 0)) *
				popMatrix * Matrix4::Translation(l.GetPosition()) *
				Matrix4::Scale(Vector3(radius, radius, radius));

			l.SetPosition(modelMatrix.GetPositionVector());
			SetShaderLight(l);
			UpdateShaderMatrices();
			float dist = (l.GetPosition() - camera->GetPosition()).Length();
			glCullFace((dist < radius) ? GL_FRONT : GL_BACK);
			sphere->Draw();
		}
	}

	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Holder::CombineBuffers() {
	SetCurrentShader(combineShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "emissiveTex"), 4);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "specularTex"), 5);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthBuffer"), 6);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, cubeMapTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
}

void Holder::GenerateScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8, width, height, 0, depth ? GL_DEPTH_COMPONENT : GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}