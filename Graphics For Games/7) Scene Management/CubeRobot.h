#pragma once
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/OBJMesh.h"

class CubeRobot : public SceneNode {
public:
	CubeRobot(void);
	~CubeRobot(void) {};

	virtual void Update(float msec);

	static void CreateCube();
	static void DeleteCube();

protected:
	static Mesh* cube;
	SceneNode* head, * leftArm, * rightArm;
};
