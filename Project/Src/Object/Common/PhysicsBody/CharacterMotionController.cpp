#include "CharacterMotionController.h"

#include <algorithm>
#include <cmath>

#pragma region 移動制御

// 移動入力に応じて水平方向の速度を更新
void CharacterMotionController::ApplyInput(
    PhysicsBody& body,
    const Vector3& inputDirection,
    float timeScale)
{
    // 物理停止中・時間停止中は処理しない
    if (body.IsFrozen() || timeScale <= 0.0f) return;

    // 現在速度を取得
    Vector3 v = body.GetVelocity();

    // 水平方向の速度を取得
    Vector3 horizontal(v.x, 0.0f, v.z);

    // 入力方向からY成分を除外
    Vector3 input(
        inputDirection.x,
        0.0f,
        inputDirection.z
    );

    // 入力方向の長さを取得
    const float inputLength = input.Length();

    // 入力方向の長さを最大1.0fに制限
    if (inputLength > 1.0f)
    {
        input *= (1.0f / inputLength);
    }

    // 現在の水平移動速度を取得
    const float speed = horizontal.Length();

    // 高速移動中か判定
    const bool highSpeed = speed > momentumThreshold;

    // 高速移動中は方向修正倍率を適用
    const float steering =
        highSpeed ? momentumSteering : 1.0f;

    // 接地状態に応じた加速度を計算
    const float accel =
        (grounded ? groundAcceleration : airAcceleration)
        * steering * timeScale;

    // 高速移動中の場合
    if (highSpeed)
    {
        // 入力がある場合のみ弱い加速度を加算
        if (input.Length() > 0.0001f)
        {
            horizontal += input * accel;
        }
    }
    // 通常移動中の場合
    else
    {
        // 入力方向から目標速度を計算
        const Vector3 target = input * moveSpeed;

        // 現在速度から目標速度への差を計算
        Vector3 delta = target - horizontal;

        // 速度差の大きさを取得
        const float len = delta.Length();

        // 1フレームあたりの加速度に制限
        if (len > accel && len > 0.0001f)
        {
            delta *= (accel / len);
        }

        // 目標速度へ向けて加減速
        horizontal += delta;
    }

    // 水平方向の速度を反映
    v.x = horizontal.x;
    v.z = horizontal.z;

    // 更新した速度を設定
    body.SetVelocity(v);
}

// 接地中ならジャンプ速度を設定
bool CharacterMotionController::TryJump(PhysicsBody& body)
{
    // 非接地・物理停止中はジャンプしない
    if (!grounded || body.IsFrozen()) return false;

    // 現在速度を取得
    Vector3 v = body.GetVelocity();

    // 上方向の速度をジャンプ速度に変更
    v.y = jumpSpeed;

    // 更新した速度を設定
    body.SetVelocity(v);

    // 接地状態を解除
    grounded = false;

    // ジャンプ成功
    return true;
}

#pragma endregion
