#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>

class SceneNode {
public:
	SceneNode(Mesh* m = nullptr, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	const Matrix4& GetTransform() const { return transform; }
	void SetTransform(const Matrix4& matrix) { transform = matrix; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	Matrix4 GetWorldTransform() const { return worldTransform; }

	// Tool Methods
	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b);
	void AddChild(SceneNode* s);
	virtual void Update(float msec);
	virtual void Draw();
	std::vector <SceneNode*>::const_iterator GetChildIteratorStart() { return children.begin(); }
	std::vector <SceneNode*>::const_iterator GetChildIteratorEnd() { return children.end(); }

protected:
	float distanceFromCamera, boundingRadius;
	SceneNode* parent;
	Mesh* mesh;
	Matrix4 worldTransform, transform;
	Vector3 modelScale;
	Vector4 colour;
	std::vector <SceneNode*> children;
};