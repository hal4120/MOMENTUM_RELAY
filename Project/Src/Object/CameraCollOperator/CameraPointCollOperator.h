#pragma once

#include "../Common/ActorBase/ActorBase.h"

class CameraBase;

class CameraPointCollOperator : public ActorBase
{
public:

	CameraPointCollOperator(const CameraBase& camera, float radius = 10.0f);
	CameraPointCollOperator(const Vector3& cameraPos, float radius = 10.0f);
	~CameraPointCollOperator()override = default;

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

	void SubUpdate(void)override;
};