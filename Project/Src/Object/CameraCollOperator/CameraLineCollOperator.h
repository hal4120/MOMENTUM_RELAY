#pragma once

#include "../Common/ActorBase/ActorBase.h"

class CameraBase;

class CapsuleCollider;

class CameraLineCollOperator : public ActorBase
{
public:
	CameraLineCollOperator(const CameraBase& camera, const Vector3& cameraFollowPos, float radius = 50.0f);
	CameraLineCollOperator(const Vector3& cameraPos, const Vector3& cameraFollowPos, float radius = 50.0f);

	~CameraLineCollOperator()override = default;

	void Load(void)override {};
	void Init(void)override {
		SetDynamicFlg(true);

		SetGravityFlg(false);

		SetPushFlg(false);
	};
	void Draw(void)override {};
	void Release(void)override {}

private:

	const Vector3& cameraPos;
	const Vector3& cameraFollowPos;

	CapsuleCollider* capsuleCollider;

	void SubUpdate(void)override;

};