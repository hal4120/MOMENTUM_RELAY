#pragma once

#include "DebugObjectBase.h"

#include "../Collider/BoxCollider.h"

class BoxDebugObject : public DebugObjectBase
{
public:
	BoxDebugObject(
		const Vector3& size,

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
		size(size)
	{
	}
	~BoxDebugObject()override = default;

	void Load(void)override {
		AddCollider(new BoxCollider(COLLIDER_TAG::DebugObject, size));
	}

private:
	Vector3 size;

	void SubDraw(void)override {
		DrawCube3D(
			(trans.pos - (size * 0.5f)).ToVECTOR(),
			(trans.pos + (size * 0.5f)).ToVECTOR(),
			0xffffff, 0xffffff, true
		);
	}
};