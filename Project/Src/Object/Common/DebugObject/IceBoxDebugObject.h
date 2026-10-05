#pragma once

#include "DebugObjectBase.h"

#include "../Collider/BoxCollider.h"

class IceBoxDebugObject : public DebugObjectBase
{
public:
	IceBoxDebugObject(
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
	~IceBoxDebugObject()override = default;

	void Load(void)override {
		AddCollider(new BoxCollider(COLLIDER_TAG::IceStage, size));
	}

private:
	Vector3 size;

	void SubDraw(void)override {
		SetUseLighting(false);
		DrawCube3D(
			(trans.pos - (size * 0.5f)).ToVECTOR(),
			(trans.pos + (size * 0.5f)).ToVECTOR(),
			0x1af7fb, 0x1af7fb, true
		);
		SetUseLighting(true);
	}
};