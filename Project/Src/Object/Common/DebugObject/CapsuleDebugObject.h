#pragma once

#include "DebugObjectBase.h"

#include "../Collider/CapsuleCollider.h"

#include "../../../Utility/Utility.h"

class CapsuleDebugObject : public DebugObjectBase
{
public:
	CapsuleDebugObject(
		const Vector3& startPos,
		const Vector3& endPos,
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
		startPos(startPos),
		endPos(endPos),
		radius(radius)
	{
	}
	~CapsuleDebugObject()override = default;

	void Load(void)override {
		AddCollider(new CapsuleCollider(COLLIDER_TAG::DebugObject, startPos, endPos, radius));
	}

private:
	Vector3 startPos, endPos;
	float radius;

	void SubDraw(void)override {

		MATRIX rotationMat = trans.RotationMat();
		DrawCapsule3D(
			(trans.pos + startPos.TransMat(rotationMat)).ToVECTOR(),
			(trans.pos + endPos.TransMat(rotationMat)).ToVECTOR(),
			radius,
			16,
			0xffffff,
			0xffffff,
			true
		);
	}
};