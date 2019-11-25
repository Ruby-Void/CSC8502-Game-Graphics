#include "Skybox.h"

Skybox::Skybox(void) {	
	skyboxCubeMap = (SOIL_load_OGL_cubemap(TEXTUREDIR"Skybox/orbital-element_ft.tga", TEXTUREDIR"Skybox/orbital-element_bk.tga",
		TEXTUREDIR"Skybox/orbital-element_up.tga", TEXTUREDIR"Skybox/orbital-element_dn.tga",
		TEXTUREDIR"Skybox/orbital-element_rt.tga", TEXTUREDIR"Skybox/orbital-element_lf.tga",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0));

	skyboxShader = new Shader(VERTEXDIR"SkyboxVertex.glsl", FRAGMENTDIR"SkyboxFragment.glsl");

	skyboxInit = (!skyboxCubeMap || !skyboxShader->LinkProgram()) ? false : true;
}

Skybox::~Skybox(void) {
	delete skyboxShader;
}