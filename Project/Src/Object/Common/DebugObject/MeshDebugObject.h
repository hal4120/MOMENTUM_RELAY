#pragma once

#include <string>

#include "DebugObjectBase.h"

#include "../Collider/MeshCollider.h"

class MeshDebugObject : public DebugObjectBase
{
public:
	MeshDebugObject(
		std::string modelPath,
		const Vector3& pos = Vector3(),
		const Quaternion& rotation = Quaternion(),
		const Vector3& scale = Vector3(1),

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
		)
	{
		trans.rotation = rotation;
		trans.scale = scale;
		trans.LoadModel(modelPath);
	}
	~MeshDebugObject() = default;

	void Load(void)override {
		AddCollider(new MeshCollider(COLLIDER_TAG::DebugObject, trans.model));
	}

private:

};