#include "Frustum.h"

void Frustum::FromMatrix(const Matrix4& mvp) {
	Vector3 xaxis = Vector3(mvp.values[0], mvp.values[4], mvp.values[8]);
	Vector3 yaxis = Vector3(mvp.values[1], mvp.values[5], mvp.values[9]);
	Vector3 zaxis = Vector3(mvp.values[2], mvp.values[6], mvp.values[10]);
	Vector3 waxis = Vector3(mvp.values[3], mvp.values[7], mvp.values[11]);
	
	planes[0] = Plane(waxis - xaxis, (mvp.values[15] - mvp.values[12]), true); // RIGHT	
	planes[1] = Plane(waxis + xaxis, (mvp.values[15] + mvp.values[12]), true); // LEFT	
	planes[2] = Plane(waxis + yaxis, (mvp.values[15] + mvp.values[13]), true); // BOTTOM	
	planes[3] = Plane(waxis - yaxis, (mvp.values[15] - mvp.values[13]), true); // TOP	
	planes[4] = Plane(waxis - zaxis, (mvp.values[15] - mvp.values[14]), true); // FAR	
	planes[5] = Plane(waxis + zaxis, (mvp.values[15] + mvp.values[14]), true); // NEAR
}

bool Frustum::InsideFrustum(SceneNode& n) {
	for (int p = 0; p < 6; ++p) {
		if (!planes[p].SphereInPlane(n.GetWorldTransform().
			GetPositionVector(), n.GetBoundingRadius())) {
			return false; // scenenode is outside this plane !
		}
	}
	return true; // Scenenode is inside every plane ...
}
