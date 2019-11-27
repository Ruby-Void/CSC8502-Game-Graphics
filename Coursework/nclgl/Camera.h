#pragma once
#include "Window.h"
#include "Matrix4.h"
#include "Vector3.h"

class Camera {
public:
	Camera(void) : yaw(0.0f), pitch(0.0f), roll(0.0f) {};

	Camera(float pitch, float yaw, float roll, Vector3 position) : 
		pitch(pitch), yaw(yaw), roll(roll), position(position) {}

	~Camera(void) {};

	void UpdateCamera(float msec = 10.0f);

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetYaw()   const { return yaw; }
	void SetYaw(float y) { yaw = y; }

	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }

	float GetRoll() const { return roll; }
	void SetRoll(float r) { roll = r; }

	bool GetFreeCamera() const { return freeCamera; }
	void SetFreeCamera(bool free) { freeCamera = free; }

protected:
	bool freeCamera = false, inverse = false;
	float yaw, pitch, roll;
	Vector3 position;
	Vector3 centrePoint;
};