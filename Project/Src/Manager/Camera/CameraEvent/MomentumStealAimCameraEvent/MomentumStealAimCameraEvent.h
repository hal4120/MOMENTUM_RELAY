#pragma once

#include "../CameraEventBase.h"
#include "../../../../Common/Vector3.h"
#include "../../../../Common/Quaternion.h"

class MomentumStealAimCameraEvent : public CameraEventBase
{
public:
    MomentumStealAimCameraEvent(const Vector3& playerPos,
        Quaternion& playerRot,
        const Vector3& offset = Vector3(15, 60, 0),
        float minPitch = -1.3962634f,
        float maxPitch = 1.3962634f,
        float transitionFrames = 12.0f);

    void Start(CameraBase& camera) override;
    void Update(CameraBase& camera) override;
    void End(CameraBase& camera) override;
    bool IsEnd(void) const override { return false; }

private:
    // FPS視点専用の感度倍率
    static constexpr float AIM_MOUSE_SENSI = 0.15f;
    static constexpr float AIM_CONTROLLER_SENSI = 0.3f;

    const Vector3& playerPos;
    Quaternion& playerRot;
    Vector3 offset;
    float minPitch;
    float maxPitch;
    float pitch = 0.0f;
    float yaw = 0.0f;
    Vector3 startPos;
    float transitionFrames;
    float transitionCounter = 0.0f;
};
