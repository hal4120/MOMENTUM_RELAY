#include "MomentumIndicator.h"

#include <algorithm>
#include <cmath>

#include "../../../Manager/Camera/CurrentCamera.h"

namespace
{
    // Rotates the arrow's model-space +X axis to the requested direction.
    Quaternion RotationFromPositiveX(const Vector3& direction)
    {
        const Vector3 from(1.0f, 0.0f, 0.0f);
        const Vector3 to = direction.Normalized();
        const float dot = std::clamp(from.Dot(to), -1.0f, 1.0f);

        if (dot > 0.99999f) { return Quaternion::Identity(); }
        if (dot < -0.99999f) {
            return Quaternion::FromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), DX_PI_F);
        }

        const Vector3 axis = from.Cross(to).Normalized();
        return Quaternion::FromAxisAngle(axis, acosf(dot));
    }
}

MomentumIndicator::MomentumIndicator(const Vector3& stealVelocity) :
    ActorBase(),
    tempScreenHandle(-1),
    frameHandle(-1),
    arrowTrans(),
    uiCamera(FixedPointCamera::LookAt,
        Vector3(0.0f, 0.0f, -UI_CAMERA_DISTANCE), Vector3()),
    stealVelocity(stealVelocity)
{
}

void MomentumIndicator::Load(void)
{
    tempScreenHandle = MakeScreen(TEMP_SCREEN_SIZE.x, TEMP_SCREEN_SIZE.y, TRUE);
    frameHandle = LoadGraph("Data/Image/Game/UI/MomentumIndicatorFrame.png");
    arrowTrans.LoadModel("MomentumIndicatorArrow/MomentumIndicatorArrow");
    arrowTrans.pos = Vector3();
    arrowTrans.scale = Vector3(ARROW_SCALE);

    SetDrawType(ACTOR_DRAW_TYPE::Ui);
}

void MomentumIndicator::Update(void)
{
    if (stealVelocity.LengthSq() <= 0.000001f) { return; }

    // World direction -> camera-relative direction (including pitch and roll).
    const Vector3 relativeDirection =
        CurrentCamera::Get().GetRotation().Inversed().Rotate(stealVelocity.Normalized());
    arrowTrans.rotation = RotationFromPositiveX(relativeDirection);
}

void MomentumIndicator::Draw(void)
{
    if (frameHandle < 0) { return; }

    // Draw the frame first: its pale center is opaque and would hide the arrow.
    if (tempScreenHandle < 0 || arrowTrans.model < 0 ||
        stealVelocity.LengthSq() <= 0.000001f) {
        DrawGraph(UI_X, UI_Y, frameHandle, TRUE);
        return;
    }

    const int previousScreen = GetDrawScreen();
    SetDrawScreen(tempScreenHandle);
    ClearDrawScreen();

    // Apply the independent UI camera; do not replace CurrentCamera's pointer.
    uiCamera.Apply();
    SetCameraNearFar(1.0f, 1000.0f);
    arrowTrans.Attach();
    arrowTrans.Draw();

    SetDrawScreen(previousScreen);
    CurrentCamera::Get().Apply();

    // Composite the 200x200 UI in the same coordinates.
    DrawGraph(UI_X, UI_Y, frameHandle, TRUE);
    DrawGraph(UI_X, UI_Y, tempScreenHandle, TRUE);
}

void MomentumIndicator::Release(void)
{
    arrowTrans.Release();
    if (frameHandle >= 0) { DeleteGraph(frameHandle); frameHandle = -1; }
    if (tempScreenHandle >= 0) { DeleteGraph(tempScreenHandle); tempScreenHandle = -1; }
    uiCamera.Release();
}
