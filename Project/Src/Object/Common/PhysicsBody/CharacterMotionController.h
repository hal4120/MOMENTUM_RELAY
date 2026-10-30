#pragma once

#include "PhysicsBody.h"

// キャラクター移動制御
class CharacterMotionController
{
public:

#pragma region 接地状態

    // 接地状態を設定
    void SetGrounded(bool value) { grounded = value; }

#pragma endregion

#pragma region 移動パラメータ

    // 通常移動の最大速度を設定
    void SetMoveSpeed(float value) { moveSpeed = value; }

    // 地上での加速度を設定
    void SetGroundAcceleration(float value) { groundAcceleration = value; }

    // 空中での加速度を設定
    void SetAirAcceleration(float value) { airAcceleration = value; }

    // 高速移動中の方向修正倍率を設定
    void SetMomentumSteering(float value) { momentumSteering = value; }

    // 高速移動と判定する速度を設定
    void SetMomentumThreshold(float value) { momentumThreshold = value; }

    // ジャンプ時の上方向の速度を設定
    void SetJumpSpeed(float value) { jumpSpeed = value; }

#pragma endregion

#pragma region 移動制御

    // 移動入力に応じて水平方向の速度を更新
    void ApplyInput(
        PhysicsBody& body,
        const Vector3& inputDirection,
        float timeScale
    );

    // 接地中ならジャンプ速度を設定
    bool TryJump(PhysicsBody& body);

#pragma endregion

private:

#pragma region 移動状態

    // 接地しているか
    bool grounded = false;

#pragma endregion

#pragma region 移動パラメータ

    // 通常移動の最大速度
    float moveSpeed = 5.0f;

    // 地上での加速度
    float groundAcceleration = 0.5f;

    // 空中での加速度
    float airAcceleration = 0.12f;

    // 高速移動中の方向修正倍率
    float momentumSteering = 0.2f;

    // 高速移動と判定する速度
    float momentumThreshold = 10.0f;

    // ジャンプ時の上方向の速度
    float jumpSpeed = 12.0f;

#pragma endregion
};
