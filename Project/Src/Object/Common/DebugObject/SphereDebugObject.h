#pragma once

#include "DebugObjectBase.h"

#include "../Collider/SphereCollider.h"

class SphereDebugObject : public DebugObjectBase
{
public:
	SphereDebugObject(
		float radius,

		const Vector3& pos = Vector3(),

		bool dynamicFlg = true,
		bool isGravity = true,
		bool pushFlg = true,
		unsigned char pushWeight = 50,
		bool isOperator = false
	) :
		DebugObjectBase(
			pos,
			dynamicFlg,
			isGravity,
			pushFlg,
			pushWeight,
			isOperator
		),
		radius(radius)
	{
	}
	~SphereDebugObject()override = default;

	void Load(void)override {
		AddCollider(new SphereCollider(COLLIDER_TAG::DebugObject, radius));
	}

private:
	float radius;

	void SubDraw(void)override { DrawSphere3D(trans.pos.ToVECTOR(), radius, 16, 0xffffff, 0xffffff, true); }
};