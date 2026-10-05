#include "LookAtCameraEvent.h"

#include <algorithm>

#include "../../../TimeScale/TimeScale.h"

#include "../../CameraBase.h"

LookAtCameraEvent::LookAtCameraEvent(const Vector3& lookAtPos, const Vector3 lookAtOffset, CameraBase& camera, const Vector3& pos, const Vector3& angle, float fov, float frame) :
	CameraEventBase(),

	lookAtPos(lookAtPos),
	lookAtOffset(lookAtOffset),

	maxFrame((std::max)(frame, 1.0f)),
	currentFrame(0)
{
	camera.SetPos(pos);
	camera.SetAngle(angle);
	camera.SetFov(fov);
}

LookAtCameraEvent::LookAtCameraEvent(const Vector3& lookAtPos, const Vector3 lookAtOffset, float frame) :
	CameraEventBase(),

	lookAtPos(lookAtPos),
	lookAtOffset(lookAtOffset),

	maxFrame((std::max)(frame, 1.0f)),
	currentFrame(0)
{
}

void LookAtCameraEvent::Update(CameraBase& camera)
{
	currentFrame += TimeScale::Get();

	// カメラから注視点へ向かうベクトル
	Vector3 vec = (lookAtPos + lookAtOffset) - camera.GetPos();

	// 安全処理
	if (vec.LengthSq() <= 0.000001f) { return; }

	// 正規化
	vec.Normalize();

	// 角度を設定
	camera.SetAngle(Vector3(-asinf(std::clamp(vec.y, -1.0f, 1.0f)), atan2f(vec.x, vec.z), 0.0f));
}