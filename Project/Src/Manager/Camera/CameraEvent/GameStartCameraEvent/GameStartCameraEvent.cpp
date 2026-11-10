#include "GameStartCameraEvent.h"

#include <algorithm>

#include "../../CameraBase.h"

#include "../../../../Scene/SceneManager.h"
#include "../../../../Scene/SceneBase.h"

#include "../../../../Scene/Common/PostEffect/GoalDirection/GoalDirectionPostEffect.h"

GameStartCameraEvent::GameStartCameraEvent(AngleTag, const Vector3& startPos, const Vector3& startAngle, float startFov, float waitFrame, float moveFrame) :
    CameraEventBase(),

    startPos(startPos), startRotation(Quaternion::FromEulerXZY(startAngle)), startFov(startFov),

    endPos(), endRotation(), endFov(0.0f),

    waitFrameMax((std::max)(waitFrame, 1.0f)),
    waitFrame(0.0f),

    maxFrame((std::max)(moveFrame, 1.0f)),
    moveFrame(0.0f)
{
}

GameStartCameraEvent::GameStartCameraEvent(LookAtTag, const Vector3& startPos, const Vector3& startLookPos, float startFov, float waitFrame, float moveFrame) :
    CameraEventBase(),

    startPos(startPos), startRotation(CalcCameraRotation(startPos, startLookPos)), startFov(startFov),

    endPos(), endRotation(), endFov(0.0f),

    waitFrameMax((std::max)(waitFrame, 1.0f)),
    waitFrame(0.0f),

    maxFrame((std::max)(moveFrame, 1.0f)),
    moveFrame(0.0f)
{
}

GameStartCameraEvent::GameStartCameraEvent(const CameraBase& camera, const Vector3& startLocalPos, const Vector3& startLocalAngle, float startLocalFov, float waitFrame, float moveFrame) :
    CameraEventBase(),

    startPos(camera.GetPos() + startLocalPos), startRotation(Quaternion::FromEulerXZY(startLocalAngle)* camera.GetRotation()), startFov(camera.GetFov() + startLocalFov),

    endPos(), endRotation(), endFov(0.0f),

    waitFrameMax((std::max)(waitFrame, 1.0f)),
    waitFrame(0.0f),

    maxFrame((std::max)(moveFrame, 1.0f)),
    moveFrame(0.0f)
{
}

void GameStartCameraEvent::Start(CameraBase& camera)
{
    // 通常Cameraが本来いる位置を保存
    endPos = camera.GetPos();
    endRotation = camera.GetRotation();
    endFov = camera.GetFov();

    // 一旦イベント開始位置へ移動
    camera.SetPos(startPos);
    camera.SetRotation(startRotation);
    camera.SetFov(startFov);

	// ゴール方向のポストエフェクトを追加
    SceneManager::GetIns().GetScene()->AddPostEffect(new GoalDirectionPostEffect());

    waitFrame = 0.0f;
}

void GameStartCameraEvent::Update(CameraBase& camera)
{
    if (waitFrame < waitFrameMax) {
        waitFrame += camera.GetTimeScale();

        if (waitFrame >= waitFrameMax) {
            waitFrame = waitFrameMax;
            moveFrame = 0.0f;

            SceneManager::GetIns().GetScene()->RemovePostEffect<GoalDirectionPostEffect>();
        }

        return;
    }

    moveFrame += camera.GetTimeScale();

    float rate = moveFrame / maxFrame;

    rate = std::clamp(rate, 0.0f, 1.0f);

    // イージング
    float t = EaseInOut(rate);

    // 座標補間
    Vector3 pos = startPos + (endPos - startPos) * t;

    // 角度補間
    Quaternion rotation = Quaternion::Slerp(startRotation, endRotation, t);

    // FOV補間
    float fov = startFov + (endFov - startFov) * t;

    camera.SetPos(pos);
    camera.SetRotation(rotation);
    camera.SetFov(fov);
}

void GameStartCameraEvent::End(CameraBase& camera)
{
    camera.SetPos(endPos);
    camera.SetRotation(endRotation);
    camera.SetFov(endFov);
}

float GameStartCameraEvent::EaseInOut(float t)
{
    return t * t * (3.0f - 2.0f * t);
}