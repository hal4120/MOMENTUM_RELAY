#pragma once

#include "MotionControllerBase.h"

#include "../Transform/Transform.h"

// キャラクター移動制御
class GroundMotionController : public MotionControllerBase
{
public:

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

    // 高速移動中、入力を離した際の接地減速度（速度/フレーム）
    void SetMomentumReleaseDeceleration(float value) { momentumReleaseDeceleration = value > 0.0f ? value : 0.0f; }

    // 接地中の水平減速度（速度/フレーム）
    void SetGroundDrag(float value) { groundDrag = value > 0.0f ? value : 0.0f; }

    // 空中の水平減速度（速度/フレーム）
    void SetAirDrag(float value) { airDrag = value > 0.0f ? value : 0.0f; }

    // ジャンプ時の上方向の速度を設定
    void SetJumpSpeed(float value) { jumpSpeed = value; }

#pragma endregion

#pragma region 移動制御

    // 入力状態に関係なく、毎フレーム水平速度を減衰させる。
    void Update(float timeScale) override;

    // 移動入力に応じて水平方向の速度を更新
    void ApplyInput(const Vector3& inputDirection, float timeScale = TimeScale::Get()) override;

    void SetTransform(Transform* value) { transform = value; }

    // 回避時に速度を付与。接地中のみY成分も加算（通常移動の速度上限は適用しない）
    void ApplyRollingImpulse(const Vector3& impulse);

    // 接地中ならジャンプ速度を設定
    bool TryJump();

    // CharacterBaseが持つ接地状態を一度だけ登録する（所有しない）
    void SetGroundedSource(const bool* value) { groundedSource = value; }
    bool IsGrounded() const { return groundedSource && *groundedSource; }

#pragma endregion

private:

    const bool* groundedSource = nullptr;

    Transform* transform = nullptr; // CharacterBaseが所有

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

    // 勢いを残しつつ、入力を離したときに停止できるようにする
    float momentumReleaseDeceleration = 0.12f;

    // 旧ActorBaseの減速度を維持する。
    float groundDrag = 0.65f;
    float airDrag = 0.08f;

    // ジャンプ時の上方向の速度
    float jumpSpeed = 12.0f;

#pragma endregion
};
