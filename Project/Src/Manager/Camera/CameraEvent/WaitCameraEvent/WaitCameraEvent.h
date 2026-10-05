#pragma once

#include "../CameraEventBase.h"

class WaitCameraEvent : public CameraEventBase
{
public:
	WaitCameraEvent(float frame);

	~WaitCameraEvent()override = default;

	void Update(CameraBase& camera) override;

    bool IsEnd() const override { return currentFrame >= maxFrame; }

private:
	// 待ちフレーム数
	const float maxFrame;
	// 更新フレーム数
	float currentFrame;
};