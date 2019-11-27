#include "Camera.h"
#include <iostream>

void Camera::UpdateCamera(float msec) {			
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_I)) {
		inverse = !inverse;
	}
	if (freeCamera) {
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);

		pitch = min(pitch, 90.0f);
		pitch = max(pitch, -90.0f);

		if (yaw < 0) { 
			yaw += 360.0f; 
		}
		if (yaw > 360.0f) { 
			yaw -= 360.0f; 
		}

		msec *= 2.5f;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_CONTROL)) {
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
				position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec;
			}
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
				position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec;
			}
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
				position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec;
			}
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
				position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec;
			}
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
				position.y += msec;
			}
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
				position.y -= msec;
			}
		}
		else {
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
				position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec * 0.25;
			}
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
				position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec * 0.25;
			}
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
				position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec * 0.25;
			}
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
				position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec * 0.25;
			}
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
				position.y += msec * 0.5;
			}
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
				position.y -= msec * 0.5;
			}
		}
	}
	else {		
		Vector3 camFocusVector = Vector3(257.0f * 16.0f / 2.0f, 1000.0f, 257.0f * 16.0f / 2.0f);
		float rotateYaw = inverse ? -0.15f : 0.15f;
		position = (Matrix4::Rotation(rotateYaw, Vector3(0, 1, 0)) * (position - camFocusVector) + camFocusVector);		

		yaw += rotateYaw;

		if (yaw < 0) {
			yaw += 360.0f;
		}
		if (yaw > 360.0f) {
			yaw -= 360.0f;
		}
	}	
}

Matrix4 Camera::BuildViewMatrix() {
	return	Matrix4::Rotation(-pitch, Vector3(1,0,0)) * Matrix4::Rotation(-yaw, Vector3(0,1,0)) * Matrix4::Translation(-position);
};