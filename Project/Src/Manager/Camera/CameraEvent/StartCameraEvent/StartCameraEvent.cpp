#include "StartCameraEvent.h"

#include <algorithm>

#include "../../../TimeScale/TimeScale.h"

#include "../../CameraBase.h"

StartCameraEvent::StartCameraEvent(const Vector3& startPos, const Vector3& startAngle, float startFov, float frame) :
    CameraEventBase(),

    startPos(startPos), startAngle(startAngle), startFov(startFov),

    endPos(), endAngle(), endFov(0.0f),

    maxFrame((std::max)(frame, 1.0f)),
    currentFrame(0)
{
}

StartCameraEvent::StartCameraEvent(const CameraBase& camera, const Vector3& startLocalPos, const Vector3& startLocalAngle, float startLocalFov, float frame) :
    CameraEventBase(),

    startPos(camera.GetPos() + startLocalPos), startAngle(camera.GetAngle() + startLocalAngle), startFov(camera.GetFov() + startLocalFov),

    endPos(), endAngle(), endFov(0.0f),

    maxFrame((std::max)(frame, 1.0f)),
    currentFrame(0)
{
}

void StartCameraEvent::Start(CameraBase& camera)
{
    // 通常Cameraが本来いる位置を保存
    endPos = camera.GetPos();
    endAngle = camera.GetAngle();
    endFov = camera.GetFov();

    // 一旦イベント開始位置へ移動
    camera.SetPos(startPos);
    camera.SetAngle(startAngle);
    camera.SetFov(startFov);

    currentFrame = 0;
}

void StartCameraEvent::Update(CameraBase& camera)
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

void StartCameraEvent::End(CameraBase& camera)
{
    camera.SetPos(endPos);
    camera.SetAngle(endAngle);
    camera.SetFov(endFov);
}

float StartCameraEvent::EaseInOut(float t)
{
    return t * t * (3.0f - 2.0f * t);
}