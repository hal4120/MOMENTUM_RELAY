#pragma once

#include "../../../Common/Vector2I.h"
#include "../../Common/ActorBase/ActorBase.h"
#include "../../../Manager/Camera/FixedPoint/FixedPointCamera.h"

class MomentumIndicator : public ActorBase
{
public:
    explicit MomentumIndicator(const Vector3& stealVelocity);
    ~MomentumIndicator() override = default;

    void Load(void) override;
    void Update(void) override;
    void Draw(void) override;
    void Release(void) override;

private:
    const Vector2I TEMP_SCREEN_SIZE = Vector2I(200, 200);
    static constexpr int UI_X = 10;
    static constexpr int UI_Y = 600;
    static constexpr float ARROW_SCALE = 0.75f; // Adjust for the actual model size
    static constexpr float UI_CAMERA_DISTANCE = 200.0f;

    int tempScreenHandle;
    int frameHandle;
    Transform arrowTrans;
    FixedPointCamera uiCamera;
    const Vector3& stealVelocity;
};
