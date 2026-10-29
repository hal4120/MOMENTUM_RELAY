#include "MomentumStealAimCameraEvent.h"

#include "../../CameraBase.h"
#include "../../../Input/InputManager.h"

#include <algorithm>
#include <cmath>

MomentumStealAimCameraEvent::MomentumStealAimCameraEvent(
    const Vector3& playerPos, Quaternion& playerRot,
    const Vector3& offset, float minPitch, float maxPitch,
    float transitionFrames)
    : playerPos(playerPos), playerRot(playerRot), offset(offset),
      minPitch(minPitch), maxPitch(maxPitch),
    transitionFrames((std::max)(0.0f, transitionFrames))
{
}

void MomentumStealAimCameraEvent::Start(CameraBase& camera)
{
    // 開始時の視線を維持する。Euler XZY の X は下向きが正。
    const Vector3 angle = camera.GetRotation().ToEulerXZY();
    pitch = (std::max)(minPitch, (std::min)(angle.x, maxPitch));
    yaw = angle.y;
    startPos = camera.GetPos();
    transitionCounter = 0.0f;
    // 座標はUpdateで補間するため、Startでは瞬間移動させない。
    camera.SetRotation(Quaternion::FromEulerXZY(Vector3(pitch, yaw, 0.0f)));
}

void MomentumStealAimCameraEvent::Update(CameraBase& camera)
{
    // 回転入力取得
    Vector3 input = Input::GetIns().GetRightStickVec(false).ToVector3YX()
        * CameraBase::GetControllerSensi()
        * AIM_CONTROLLER_SENSI;

    if (input == 0.0f) {
        input = Input::GetIns().GetMouseMoveVec().ToVector3YX()
            * CameraBase::GetMouseSensi()
            * AIM_MOUSE_SENSI;
    }

    if (input == 0.0f) {
        if (Input::GetIns().GetInfo(KEY_TYPE::CameraRotLeft).now) { input.y--; }
        if (Input::GetIns().GetInfo(KEY_TYPE::CameraRotRight).now) { input.y++; }
        if (Input::GetIns().GetInfo(KEY_TYPE::CameraRotUp).now) { input.x--; }
        if (Input::GetIns().GetInfo(KEY_TYPE::CameraRotDown).now) { input.x++; }

        if (input != 0.0f) { input.Normalize(); }
    }

    // 回転更新（タイムスケール非依存）
    pitch = (std::max)(minPitch, (std::min)(pitch + input.x, maxPitch));
    yaw += input.y;

    // カメラの回転
    const Quaternion cameraRotation =
        Quaternion::FromEulerXZY(Vector3(pitch, yaw, 0.0f));

    // プレイヤーの回転も同時に更新
    playerRot = cameraRotation;

    // カメラの目標座標
    const Vector3 desiredPos =
        playerPos + cameraRotation.Rotate(offset);

    // 開始時の補間
    if (transitionFrames > 0.0f && transitionCounter < transitionFrames) {

        transitionCounter =
            (std::min)(transitionCounter + 1.0f, transitionFrames);

        const float t = transitionCounter / transitionFrames;
        const float eased = t * t * (3.0f - 2.0f * t);

        camera.SetPos(startPos + (desiredPos - startPos) * eased);
    }
    else {
        camera.SetPos(desiredPos);
    }

    // カメラの回転を適用
    camera.SetRotation(cameraRotation);
}

void MomentumStealAimCameraEvent::End(CameraBase& camera)
{
    // イベント終了後のカメラ姿勢は保持し、通常カメラへ制御を戻す。
}
