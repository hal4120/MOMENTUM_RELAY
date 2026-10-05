#include "MoveCameraEvent.h"

#include <algorithm>

#include "../../../TimeScale/TimeScale.h"

#include "../../CameraBase.h"

MoveCameraEvent::MoveCameraEvent(const Vector3& endPos, const Vector3& endAngle, float endFov, float frame) :
	CameraEventBase(),

	startPos(), startAngle(), startFov(0.0f),

	endPos(endPos), endAngle(endAngle), endFov(endFov),

	maxFrame((std::max)(frame, 1.0f)),
	currentFrame(0)
{
}

MoveCameraEvent::MoveCameraEvent(const CameraBase& camera, const Vector3& endLocalPos, const Vector3& endLocalAngle, float endLocalFov, float frame) :
    CameraEventBase(),

    startPos(), startAngle(), startFov(0.0f),

    endPos(camera.GetPos() + endLocalPos), endAngle(camera.GetAngle() + endLocalAngle), endFov(camera.GetFov() + endLocalFov),

    maxFrame((std::max)(frame, 1.0f)),
    currentFrame(0)
{
}

void MoveCameraEvent::Start(CameraBase& camera)
{
	startPos = camera.GetPos();
	startAngle = camera.GetAngle();
	startFov = camera.GetFov();

	currentFrame = 0;
}

void MoveCameraEvent::Update(CameraBase& camera)
{
    currentFrame += TimeScale::Get();

    float rate = currentFrame / maxFrame;

    rate = std::clamp(rate, 0.0f, 1.0f);

    // イージング
    float t = EaseInOut(rate);

    // 座標補間
    Vector3 pos = startPos + (endPos - startPos) * t;

    // 角度補間
    Vector3 angle = startAngle + (endAngle - startAngle) * t;

    // FOV補間
    float fov = startFov + (endFov - startFov) * t;

    camera.SetPos(pos);
    camera.SetAngle(angle);
    camera.SetFov(fov);
}

void MoveCameraEvent::End(CameraBase& camera)
{
    camera.SetPos(endPos);
    camera.SetAngle(endAngle);
    camera.SetFov(endFov);
}

float MoveCameraEvent::EaseInOut(float t)
{
    return t * t * (3.0f - 2.0f * t);
}
