#pragma once

#include "../CameraEventBase.h"

#include "../../../../Common/Vector3.h"

class CameraBase;

class LookAtCameraEvent : public CameraEventBase
{
public:
	LookAtCameraEvent(const Vector3& lookAtPos, const Vector3 lookAtOffset, CameraBase& camera, const Vector3& pos, const Vector3& angle, float fov, float frame);

	LookAtCameraEvent(const Vector3& lookAtPos, const Vector3 lookAtOffset, float frame);

	~LookAtCameraEvent()override = default;

	void Start(CameraBase& camera)override { Update(camera); }

	void Update(CameraBase& camera)override;

	bool IsEnd() const override { return currentFrame >= maxFrame; }

private:

	const Vector3& lookAtPos;
	// ’‹“_‚ğŠ„‚èo‚·’Ç]‘ÎÛ‚©‚ç‚Ì‘Š‘ÎÀ•W
	const Vector3 lookAtOffset;

	// ŠÔ
	const float maxFrame;
	float currentFrame;
};