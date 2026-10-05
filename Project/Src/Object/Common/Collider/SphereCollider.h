#pragma once

#include"ColliderBase.h"

class SphereCollider : public ColliderBase
{
public:
	SphereCollider(COLLIDER_TAG tag, float radius, const Vector3& pos = Vector3()) :
		ColliderBase(tag, pos),
		radius(radius)
	{
		SetShape(COLLIDER_SHAPE::Sphere);
	}
	~SphereCollider()override {}

	float GetRadius(void)const { return radius; }
	void SetRadius(float radius) { this->radius = radius; }

	AABB GetAABB(void) const override {
		Vector3 p = GetPos();
		Vector3 r = radius;

		return AABB(p - r, p + r);
	}

	void DrawDebug(unsigned int color = 0xffffff)override {
		DrawSphere3D(GetPos().ToVECTOR(), radius, 12, color, color, true);
	}

private:
	float radius;
};