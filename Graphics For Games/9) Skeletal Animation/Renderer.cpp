#include "Renderer.h"
#include <random>

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	camera = new Camera(0, -90.0f, 0, Vector3(-180, 60, 0));
	currentShader = new Shader(SHADERDIR"SkeletonVertex.glsl", SHADERDIR"TexturedFragment.glsl");

	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");

	for (int i = 0; i < 5; i++) {
		hellNode.push_back(new MD5Node(*hellData));
	}

	if (!currentShader->LinkProgram()) { return; }

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	random_device rd;
	mt19937 generation(rd());	
	uniform_int_distribution<> distr(0, hellData->GetAnim(MESHDIR"idle2.md5anim")->GetNumFrames());

	for (auto node : hellNode) {
		node->PlayAnim(MESHDIR"idle2.md5anim", distr(generation));
	}

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	init = true;
}

Renderer::~Renderer(void) {
	delete camera;
	delete hellData;
	for (auto node : hellNode) {
		delete node;
	}
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	for (auto node : hellNode) {
		node->Update(msec);
	}
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	UpdateShaderMatrices();

	/*for (int y = 0; y < 10; ++y) {
		for (int x = 0; x < 10; ++x) {
			modelMatrix = Matrix4::Translation(Vector3(x * 100, 0, y * 100));
			UpdateShaderMatrices();
			hellNode->Draw(*this);
		}
	}*/

	int y = 0;
	int x = 0;
	for (auto node : hellNode) {
		modelMatrix = Matrix4::Translation(Vector3(x * 100, 0, y * 100));
		UpdateShaderMatrices();
		node->Draw(*this);
		y++;
	}

	glUseProgram(0);
	SwapBuffers();
}