#include "Enviroment.h"

Mesh* Enviroment::landscape = nullptr;
vector<Mesh*> Enviroment::flora = vector<Mesh*>();

Enviroment::Enviroment(void) {
	SceneNode* land = new SceneNode(landscape);
	AddChild(land);
}

void Enviroment::Update(float msec) {
	SceneNode::Update(msec);
}

void Enviroment::CreateLandscape(std::string meshLocation) {
	OBJMesh* land = new OBJMesh();
	land->LoadOBJMesh(meshLocation);
	land->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"bark_Diffuse.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	land->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"bark_Bump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	landscape = land;
}

void Enviroment::CreateFlora(std::string meshLocation) {
	OBJMesh* plant = new OBJMesh();
	plant->LoadOBJMesh(meshLocation);
	flora.push_back(plant);
}

void Enviroment::DeleteLandscape() {
	delete landscape;
}

void Enviroment::DeleteFlora() {
	for (auto plant : flora) {
		delete plant;
	}
}
