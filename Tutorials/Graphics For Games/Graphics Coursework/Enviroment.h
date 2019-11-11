#pragma once
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/OBJMesh.h"

class Enviroment : public SceneNode {
public:
	Enviroment(void);
	~Enviroment(void) {};

	virtual void Update(float msec);

	static void CreateLandscape(std::string meshLocation);
	static void DeleteLandscape();

	static void CreateFlora(std::string meshLocation);	
	static void DeleteFlora();

protected:
	static Mesh* landscape;
	static vector<Mesh*> flora;
	SceneNode* enviroment;
};