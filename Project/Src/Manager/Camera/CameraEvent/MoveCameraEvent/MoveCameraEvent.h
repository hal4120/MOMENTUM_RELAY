#pragma once

#include "../CameraEventBase.h"

#include "../../../../Common/Vector3.h"

class MoveCameraEvent : public CameraEventBase
{
public:

    /// <summary>
    /// 現在のカメラ情報（位置、角度、視野角）から、指定したカメラ情報（位置、角度、視野角）まで指定されたフレーム数で移動する
    /// </summary>
    /// <param name="endPos">終了座標</param>
    /// <param name="endAngle">終了角度</param>
    /// <param name="endFov">終了視野角</param>
    /// <param name="frame">移動フレーム数</param>
    MoveCameraEvent(const Vector3& endPos, const Vector3& endAngle, float endFov, float frame);

    /// <summary>
    /// 現在のカメラ情報から指定したカメラ情報分移動した情報から、現在のカメラ情報（位置、角度、視野角）まで指定されたフレーム数で移動する
    /// </summary>
    /// <param name="camera">現在のカメラ</param>
    /// <param name="startLocalPos">開始相対座標</param>
    /// <param name="startLocalAngle">開始相対角度</param>
    /// <param name="startLocalFov">開始相対視野角</param>
    /// <param name="frame">移動フレーム数</param>
    MoveCameraEvent(const CameraBase& camera, const Vector3& endLocalPos, const Vector3& endLocalAngle, float endLocalFov, float frame);

    ~MoveCameraEvent() override = default;

    void Start(CameraBase& camera) override;

    void Update(CameraBase& camera) override;

    void End(CameraBase& camera) override;

    bool IsEnd(void) const override { return currentFrame >= maxFrame; }

private:

    // 開始時カメラ情報
    Vector3 startPos;
    Vector3 startAngle;
    float startFov;

    // 終了時カメラ情報
    const Vector3 endPos;
    const Vector3 endAngle;
    const float endFov;

    // 時間
    const float maxFrame;
    float currentFrame;

    // 補間
    float EaseInOut(float t);
};