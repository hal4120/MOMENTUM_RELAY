#pragma once

#include "Common/ActorBase/ActorBase.h"

#include "Common/Shader/WaterShader.h"

#include "Common/Collider/BoxCollider.h"

class Water : public ActorBase
{
public:
	Water() :ActorBase() {}
	~Water()override = default;

	void Load(void)override {
		SetDynamicFlg(false);

		SetPushFlg(false);

		trans.LoadModel("Water/WaterWaveCube");

		trans.pos = Vector3(500, 100, 500);

		trans.scale = Vector3(5.0f, 1.5f, 5.0f);

		AddCollider(new BoxCollider(COLLIDER_TAG::Water, Vector3(200, 200, 200) * trans.scale));
	}

	// “–‚½‚è”»’è‚Ì’Ê’m
	void OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)override {
		if (other.GetTag() == COLLIDER_TAG::Camera) { nowIsUnderWater = true; SetIsDraw(false); }
	}

	char UnderWaterPostEffectSwitch(void) {
		if (prevIsUnderWater == nowIsUnderWater) { return -1; }
		return (char)nowIsUnderWater;
	}

private:

	void SubInit(void)override {
		CreateShader(new WaterShader());

		SetDrawType(ACTOR_DRAW_TYPE::Alpha);
	}

	void SubUpdate(void)override {
		prevIsUnderWater = nowIsUnderWater;
		nowIsUnderWater = false;

		SetIsDraw(true);
	}

	bool nowIsUnderWater = false;
	bool prevIsUnderWater = false;
};