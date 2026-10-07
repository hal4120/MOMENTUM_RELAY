#pragma once

#include "../../Object/Common/ActorBase/ActorBase.h"

class CameraBase;

class CameraCollOperator : public ActorBase
{
public:

	CameraCollOperator(CameraBase& camera, float radius);
	~CameraCollOperator()override = default;

	void Load(void)override {};

	void Init(void)override {
		SetDynamicFlg(true);

		SetGravityFlg(false);

		SetPushFlg(false);
	};

	void Draw(void)override {};
	void Release(void)override {}

private:

	CameraBase& camera;

	void SubUpdate(void)override;
};