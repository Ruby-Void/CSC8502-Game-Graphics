/*
The Camera class currently creates view matrix rotations about the X and Y axis.
Try adding in a roll member variable to the Camera class, which rotates about the Z axis. 
Does it do what you thought it would? 
How about when you roll 90 degrees?
*/
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

protected:
	float yaw, pitch, roll;
	Vector3 position;
};