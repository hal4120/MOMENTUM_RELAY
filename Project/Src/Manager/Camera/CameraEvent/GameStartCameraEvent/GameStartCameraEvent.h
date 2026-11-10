#pragma once

#include "../CameraEventBase.h"

#include "../../../../Common/Vector3.h"
#include "../../../../Common/Quaternion.h"

#include <algorithm>

class CameraBase;

class GameStartCameraEvent : public CameraEventBase
{
public:

    struct AngleTag {};
    struct LookAtTag {};

    static constexpr AngleTag Angle{};
    static constexpr LookAtTag LookAt{};

    /// <summary>
    /// 指定したカメラ情報（位置、角度、視野角）から、現在のカメラ情報（位置、角度、視野角）まで指定されたフレーム数で移動する
    /// </summary>
    /// <param name="startPos">開始座標</param>
    /// <param name="startAngle">開始角度</param>
    /// <param name="startFov">開始視野角</param>
    /// <param name="waitFrame">待ちフレーム数</param>
    /// <param name="moveFrame">移動フレーム数</param>
    GameStartCameraEvent(AngleTag, const Vector3& startPos, const Vector3& startAngle, float startFov, float waitFrame, float moveFrame);

    /// <summary>
    /// 指定したカメラ情報（位置、注視点、視野角）から、現在のカメラ情報（位置、角度、視野角）まで指定されたフレーム数で移動する
    /// </summary>
    /// <param name="startPos">開始座標</param>
    /// <param name="startAngle">開始注視点</param>
    /// <param name="startFov">開始視野角</param>
    /// <param name="waitFrame">待ちフレーム数</param>
    /// <param name="moveFrame">移動フレーム数</param>
    GameStartCameraEvent(LookAtTag, const Vector3& startPos, const Vector3& startLookPos, float startFov, float waitFrame, float moveFrame);

    /// <summary>
    /// 現在のカメラ情報から指定したカメラ情報分移動した情報から、現在のカメラ情報（位置、角度、視野角）まで指定されたフレーム数で移動する
    /// </summary>
    /// <param name="camera">現在のカメラ</param>
    /// <param name="startLocalPos">開始相対座標</param>
    /// <param name="startLocalAngle">開始相対角度</param>
    /// <param name="startLocalFov">開始相対視野角</param>
    /// <param name="waitFrame">待ちフレーム数</param>
    /// <param name="moveFrame">移動フレーム数</param>
    GameStartCameraEvent(const CameraBase& camera, const Vector3& startLocalPos, const Vector3& startLocalAngle, float startLocalFov, float waitFrame, float moveFrame);

    ~GameStartCameraEvent()override = default;

    void Start(CameraBase& camera) override;

    void Update(CameraBase& camera) override;

    void End(CameraBase& camera) override;

    bool IsEnd() const override { return moveFrame >= maxFrame; }

private:

    const Vector3 startPos;
    const Quaternion startRotation;
    const float startFov;

    Vector3 endPos;
    Quaternion endRotation;
    float endFov;

	// 最初数秒はカメラを動かさないようにするためのフレーム数
	const float waitFrameMax;
	// 最初数秒はカメラを動かさないようにするためのフレーム数のカウンター
	float waitFrame;

    // 移動フレーム数
    const float maxFrame;
    // 更新フレーム数
    float moveFrame;

    // 補間
    float EaseInOut(float t);

    // 座標と注視点から角度を割り出す
    Quaternion CalcCameraRotation(const Vector3& pos, const Vector3& lookAtPos) {

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