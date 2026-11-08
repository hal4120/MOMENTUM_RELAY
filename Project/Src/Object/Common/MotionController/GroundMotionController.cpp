#include "GroundMotionController.h"

#include <algorithm>
#include <cmath>
#include "../../../Manager/TimeScale/TimeScale.h"

#pragma region 移動制御

// 入力のないステートも含めて、毎フレーム水平速度を減衰させる。
void GroundMotionController::Update(float timeScale)
{
    if (!physicsBody || physicsBody->IsFrozen() || timeScale <= 0.0f) return;
    if (physicsBody->GetMode() != PhysicsBody::MotionMode::Character) return;

    Vector3 velocity = physicsBody->GetVelocity();
    const float speed = std::sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
    if (speed <= 0.0f) return;

    // 接地情報は前回の衝突判定結果。Y方向の速度は変更しない。
    const float drag = (IsGrounded() ? groundDrag : airDrag) * timeScale;
    const float remaining = speed - drag;
    const float ratio = remaining > 0.0f ? remaining / speed : 0.0f;

    velocity.x *= ratio;
    velocity.z *= ratio;
    physicsBody->SetVelocity(velocity);
}


// 移動入力に応じて水平方向の速度を更新
void GroundMotionController::ApplyInput(const Vector3& inputDirection, float timeScale)
{
    if (!physicsBody) return;
    PhysicsBody& body = *physicsBody;
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

    const bool grounded = IsGrounded();

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
        else if (grounded && momentumReleaseDeceleration > 0.0f)
        {
            // 勢いはすぐに消さず、接地中だけ一定量ずつ減速する。
            // 空中では従来どおり慣性を保持する。
            const float deceleration = momentumReleaseDeceleration * timeScale;
            const float remaining = speed - deceleration;
            horizontal *= remaining > 0.0f ? remaining / speed : 0.0f;
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

    // 入力方向へキャラクターの向きを補間
    if (transform && input.LengthSq() > 0.0001f) {
        const float angle = atan2f(input.x, input.z);
        transform->SetRotation(Quaternion::Slerp(
            transform->rotation, Quaternion::FromRotationY(angle),
            0.25f * timeScale));
    }

    // 水平方向の速度を反映
    v.x = horizontal.x;
    v.z = horizontal.z;

    // 更新した速度を設定
    body.SetVelocity(v);
}

// 回避時の瞬間的な加速（通常の移動入力とは独立）
void GroundMotionController::ApplyRollingImpulse(const Vector3& impulse)
{
    if (!physicsBody || physicsBody->IsFrozen()) return;
    const float timeScale = TimeScale::Get();
    if (timeScale <= 0.0f) return;

    // 接地中だけ上方向の速度も付与する。空中では水平成分のみ。
    const float lift = IsGrounded() ? impulse.y : 0.0f;
    physicsBody->AddVelocity(Vector3(impulse.x, lift, impulse.z) * timeScale);
}

// 接地中ならジャンプ速度を設定
bool GroundMotionController::TryJump()
{
    if (!physicsBody) return false;
    PhysicsBody& body = *physicsBody;
    // 非接地・物理停止中はジャンプしない
    if (!IsGrounded() || body.IsFrozen()) return false;

    // 現在速度を取得
    Vector3 v = body.GetVelocity();

    // 上方向の速度をジャンプ速度に変更
    v.y = jumpSpeed;

    // 更新した速度を設定
    body.SetVelocity(v);

    // ジャンプ成功
    return true;
}

#pragma endregion
