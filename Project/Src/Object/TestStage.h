#pragma once

#include "Common/ActorBase/ActorBase.h"

#include "Common/Collider/MeshCollider.h"

class TestStage : public ActorBase
{
public:
	TestStage() :
		ActorBase()
	{
	}

	~TestStage()override = default;

	void Load(void)override {

		SetDynamicFlg(false);

		SetPushFlg(true);

		trans.LoadModel("TestStage/TestStage");

		trans.pos = Vector3(-500, -1000, -500);

		trans.scale = 50;

		AddCollider(new MeshCollider(COLLIDER_TAG::Stage));
	}

private:

	void SubUpdate(void)override {

		static bool prevDebugKey = false, nowDebugKey = false, isPostEffect = false;
		prevDebugKey = nowDebugKey;
		nowDebugKey = CheckHitKey(KEY_INPUT_R) == 1;
		if (!prevDebugKey && nowDebugKey) {
			SetIsDraw();
		}

	}
};