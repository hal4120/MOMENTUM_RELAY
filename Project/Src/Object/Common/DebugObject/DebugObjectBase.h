#pragma once

#include "../PhysicsBody/PhysicsBody.h"
#include "../../../Manager/TimeScale/TimeScale.h"
#include <cmath>

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

		bool isOperator
	) :
		ActorBase(),
		isOperator(isOperator)
	{
		trans.pos = pos;

		SetColliderMobility(dynamicFlg ? COLLIDER_MOBILITY::Movable : COLLIDER_MOBILITY::Static);

		// 動的オブジェクトとして生成された場合
		if (dynamicFlg) {
			// 物理挙動を生成
			EnablePhysics();

			// 重力
			GetPhysicsBody()->SetGravityEnabled(isGravity);
		}

		// 押し出し
		SetContactResponseEnabled(pushFlg);
	}
	DebugObjectBase(
		const std::string& parameterPath,

		const Vector3& pos,

		bool dynamicFlg,
		bool isGravity,
		bool pushFlg,

		bool isOperator
	) :
		ActorBase(parameterPath),
		isOperator(isOperator)
	{
		trans.pos = pos;

		SetColliderMobility(dynamicFlg ? COLLIDER_MOBILITY::Movable : COLLIDER_MOBILITY::Static);

		// 動的オブジェクトとして生成された場合
		if (dynamicFlg) {
			// 物理挙動を生成
			EnablePhysics();

			// 重力
			GetPhysicsBody()->SetGravityEnabled(isGravity);
		}

		// 押し出し
		SetContactResponseEnabled(pushFlg);
	}
	~DebugObjectBase()override = default;

	void OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)override {
		if (other.GetTag() == COLLIDER_TAG::CameraLine) { SetDrawType(ACTOR_DRAW_TYPE::Alpha); }
	}

private:
	float debugAcceleration = 3.0f;
	float debugMaxSpeed = 15.0f;

	// 操作可能かどうか
	bool isOperator;

	void SubInit(void)override {
		// 移動加速力
		debugAcceleration = 3.0f;
		// 移動減速力
		// Deceleration is handled by PhysicsBody.
		// 最大移動速度
		debugMaxSpeed = 15.0f;
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
		vec.TransMatOwn(MGetRotY(CurrentCamera::Get().GetRotation().GetYaw()));

		// 移動
		if (vec != 0.0f) {
			if (PhysicsBody* body = GetPhysicsBody()) {
				if (!body->IsFrozen()) {
					const Vector3 velocity = body->GetVelocity();
					const float speedXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
					const float steering = speedXZ > debugMaxSpeed ? 0.2f : 1.0f;
					body->AddVelocity(vec * (debugAcceleration * steering * TimeScale::Get()));
					if (speedXZ <= debugMaxSpeed) {
						Vector3 next = body->GetVelocity();
						const float nextXZ = sqrtf(next.x * next.x + next.z * next.z);
						if (nextXZ > debugMaxSpeed && nextXZ > 0.0f) {
							const float ratio = debugMaxSpeed / nextXZ;
							next.x *= ratio; next.z *= ratio;
							body->SetVelocity(next);
						}
					}
				}
			}
			const float angle = atan2f(vec.x, vec.z);
			trans.SetRotation(Quaternion::Slerp(
				trans.rotation, Quaternion::FromRotationY(angle), 0.25f * TimeScale::Get()));
		}
	}
};