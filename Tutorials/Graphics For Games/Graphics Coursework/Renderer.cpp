#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	Enviroment::CreateLandscape(MESHDIR"M_tree_small_1.obj");
	camera = new Camera();
	currentShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");
	
	if (!currentShader->LinkProgram()) { return; }

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	camera->SetPosition(Vector3(0, 30, 175));

	root = new SceneNode();
	root->AddChild(new Enviroment());
	
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}

Renderer::~Renderer(void) {
	delete camera;
	delete quad;
	delete skyboxShader;
	delete root;
	Enviroment::DeleteLandscape();
	currentShader = 0;
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	//root->Update(msec);
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glUseProgram(currentShader->GetProgram());
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 1);
	DrawNode(root);
	glUseProgram(0);
	SwapBuffers();
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);
	UpdateShaderMatrices();
	quad->Draw();
	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Matrix4 transform = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&transform);
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());
		n->Draw();
	}

	for (vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}



//quad = Mesh::GenerateQuad();
//camera->SetPosition(Vector3(0, 30, 175));
//skyboxShader = new Shader(SHADERDIR"SkyboxShaders/SkyboxVertex.glsl", SHADERDIR"SkyboxShaders/SkyboxFragment.glsl");
//if (!skyboxShader->LinkProgram()) { return; }
//
//// Sky textures courtesy of http://www.custommapmakers.org/skyboxes.php
//// Will replace with higher quality later, proof of concept currently.
//cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"Skybox/orbital-element_ft.tga", TEXTUREDIR"Skybox/orbital-element_bk.tga",
//	TEXTUREDIR"Skybox/orbital-element_up.tga", TEXTUREDIR"Skybox/orbital-element_dn.tga",
//	TEXTUREDIR"Skybox/orbital-element_rt.tga", TEXTUREDIR"Skybox/orbital-element_lf.tga",
//	SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
//
//if (!cubeMap) { return; }