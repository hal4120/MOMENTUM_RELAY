#pragma once

#include "../Common/ActorBase/ActorBase.h"

#include "../Common/Shader/WaterShader.h"

#include "../Common/Collider/BoxCollider.h"

class Water : public ActorBase
{
public:

	// 水のモデルサイズ
	static const inline Vector3 MODEL_SIZE = Vector3(200);

	Water(const Vector3& pos = Vector3(), const Vector3& size = Vector3(100)) :
		ActorBase()
	{
		// 座標を設定
		trans.pos = pos;

		// モデルのサイズと指定サイズからスケールを計算して設定
		trans.scale = size / MODEL_SIZE;
	}
	~Water()override = default;

	void Load(void)override {
		SetColliderMobility(COLLIDER_MOBILITY::Static);

		SetContactResponseEnabled(false);

		trans.LoadModel("Water/Water");

		AddCollider(new BoxCollider(COLLIDER_TAG::Water, MODEL_SIZE * trans.scale));
	}

	// 当たり判定の通知
	void OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)override {
		if (other.GetTag() == COLLIDER_TAG::CameraPoint) { nowIsUnderWater = true; SetIsDraw(false); }
	}

	char UnderWaterPostEffectSwitch(void) {
		if (prevIsUnderWater == nowIsUnderWater) { return -1; }
		return (char)nowIsUnderWater;
	}

private:

	void DefaultCreateShader(void)override { CreateShader(new WaterShader()); }

	void SubInit(void)override {
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