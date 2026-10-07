#pragma once

#include "../ActorBase/ActorBase.h"

#include "../../../Manager/Input/InputManager.h"
#include "../../../Manager/Camera/CurrentCamera.h"

#include "../Collider/ColliderBase.h"

class DebugObjectBase : public ActorBase
{
public:
	DebugObjectBase(
		const Vector3& pos,

		bool dynamicFlg,
		bool isGravity,
		bool pushFlg,
		unsigned char pushWeight,

		bool isOperator
	) :
		ActorBase(),
		isOperator(isOperator)
	{
		trans.pos = pos;
		SetDynamicFlg(dynamicFlg);
		SetGravityFlg(isGravity);
		SetPushFlg(pushFlg);
		SetPushWeight(pushWeight);
	}
	DebugObjectBase(
		const std::string& parameterPath,

		const Vector3& pos,

		bool dynamicFlg,
		bool isGravity,
		bool pushFlg,
		unsigned char pushWeight,

		bool isOperator
	) :
		ActorBase(parameterPath),
		isOperator(isOperator)
	{
		trans.pos = pos;
		SetDynamicFlg(dynamicFlg);
		SetGravityFlg(isGravity);
		SetPushFlg(pushFlg);
		SetPushWeight(pushWeight);
	}
	~DebugObjectBase()override = default;

	void OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)override {
		if (other.GetTag() == COLLIDER_TAG::CameraLine) { SetDrawType(ACTOR_DRAW_TYPE::Alpha); }
	}

private:
	// 操作可能かどうか
	bool isOperator;

	void SubInit(void)override {
		// 移動加速力
		ACCEL_RATE = 3.0f;
		// 移動減速力
		DECEL_RATE = 3.0f;
		// 最大移動速度
		ACCEL_MAX = 15.0f;
	}

	void SubUpdate(void)override {
		SetDrawType(ACTOR_DRAW_TYPE::Default);

		if (!isOperator) { return; }

		// 移動方向
		Vector3 vec = {};

		// 操作
		if (Input::GetIns().GetInfo(KEY_TYPE::DebugObjectFront).now) { vec.z++; }
		if (Input::GetIns().GetInfo(KEY_TYPE::DebugObjectBack).now) { vec.z--; }
		if (Input::GetIns().GetInfo(KEY_TYPE::DebugObjectRight).now) { vec.x++; }
		if (Input::GetIns().GetInfo(KEY_TYPE::DebugObjectLeft).now) { vec.x--; }
		if (Input::GetIns().GetInfo(KEY_TYPE::DebugObjectUp).now) { vec.y++; }
		if (Input::GetIns().GetInfo(KEY_TYPE::DebugObjectDown).now) { vec.y--; }

		// カメラの角度に合わせて移動方向を回転させる
		vec.TransMatOwn(MGetRotY(CurrentCamera::Get().GetAngle().y));

		// 移動
		MoveAccel(vec);
	}
};