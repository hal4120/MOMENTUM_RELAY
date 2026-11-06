#pragma once

#include "../CameraEventBase.h"

#include <algorithm>

#include "../../../../Common/Vector3.h"
#include "../../../../Common/Quaternion.h"

class MoveCameraEvent : public CameraEventBase
{
public:

    struct AngleTag {};
    struct LookAtTag {};

    static constexpr AngleTag Angle{};
    static constexpr LookAtTag LookAt{};

    /// <summary>
    /// 現在のカメラ情報（位置、角度、視野角）から、指定したカメラ情報（位置、角度、視野角）まで指定されたフレーム数で移動する
    /// </summary>
    /// <param name="endPos">終了座標</param>
    /// <param name="endAngle">終了角度</param>
    /// <param name="endFov">終了視野角</param>
    /// <param name="frame">移動フレーム数</param>
    MoveCameraEvent(AngleTag, const Vector3& endPos, const Vector3& endAngle, float endFov, float frame);

    /// <summary>
    /// 現在のカメラ情報（位置、注視点、視野角）から、指定したカメラ情報（位置、注視点、視野角）まで指定されたフレーム数で移動する
    /// </summary>
    /// <param name="endPos">終了座標</param>
    /// <param name="endAngle">終了注視点</param>
    /// <param name="endFov">終了視野角</param>
    /// <param name="frame">移動フレーム数</param>
    MoveCameraEvent(LookAtTag, const Vector3& endPos, const Vector3& endLookAt, float endFov, float frame);

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
    Quaternion startRotation;
    float startFov;

    // 終了時カメラ情報
    const Vector3 endPos;
    const Quaternion endRotation;
    const float endFov;

    // 時間
    const float maxFrame;
    float currentFrame;

    // 補間
    float EaseInOut(float t);

    // 座標と注視点から角度を割り出す
    static Quaternion CalcCameraRotation(const Vector3& pos, const Vector3& lookAtPos) {

        // カメラから注視点へ向かうベクトル
        Vector3 vec = lookAtPos - pos;

        // 安全処理
        if (vec.LengthSq() <= 0.000001f) { return Quaternion::Identity(); }

        // 正規化
        vec.Normalize();

        // 角度を返す
        return Quaternion::FromEulerXZY(Vector3(-asinf(std::clamp(vec.y, -1.0f, 1.0f)), atan2f(vec.x, vec.z), 0.0f));
    }
};