#pragma once

#include "../CameraEventBase.h"

#include "../../../../Common/Vector3.h"

class CameraBase;

class StartCameraEvent : public CameraEventBase
{
public:

    /// <summary>
    /// 指定したカメラ情報（位置、角度、視野角）から、現在のカメラ情報（位置、角度、視野角）まで指定されたフレーム数で移動する
    /// </summary>
    /// <param name="startPos">開始座標</param>
    /// <param name="startAngle">開始角度</param>
    /// <param name="startFov">開始視野角</param>
    /// <param name="frame">移動フレーム数</param>
    StartCameraEvent(const Vector3& startPos, const Vector3& startAngle, float startFov, float frame);

    /// <summary>
    /// 現在のカメラ情報から指定したカメラ情報分移動した情報から、現在のカメラ情報（位置、角度、視野角）まで指定されたフレーム数で移動する
    /// </summary>
    /// <param name="camera">現在のカメラ</param>
    /// <param name="startLocalPos">開始相対座標</param>
    /// <param name="startLocalAngle">開始相対角度</param>
    /// <param name="startLocalFov">開始相対視野角</param>
    /// <param name="frame">移動フレーム数</param>
    StartCameraEvent(const CameraBase& camera, const Vector3& startLocalPos, const Vector3& startLocalAngle, float startLocalFov, float frame);

    ~StartCameraEvent()override = default;

    void Start(CameraBase& camera) override;

    void Update(CameraBase& camera) override;

    void End(CameraBase& camera) override;

    bool IsEnd() const override { return currentFrame >= maxFrame; }

private:

    const Vector3 startPos;
    const Vector3 startAngle;
    const float startFov;

    Vector3 endPos;
    Vector3 endAngle;
    float endFov;

    // 移動フレーム数
    const float maxFrame;
    // 更新フレーム数
    float currentFrame;

    // 補間
    float EaseInOut(float t);
};