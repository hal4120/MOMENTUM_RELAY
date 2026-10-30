#include "PhysicsBody.h"

#include <cmath>

#pragma region 補助関数

// 0.0f～1.0fの範囲に制限
float PhysicsBody::Clamp01(float value)
{
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

#pragma endregion

#pragma region 物理パラメータ

// 1フレームあたりの水平方向の減速量を設定
void PhysicsBody::SetHorizontalDragPerFrame(float value)
{
    horizontalDragPerFrame = value < 0.0f ? 0.0f : value;
}

// 反発係数を設定
void PhysicsBody::SetRestitution(float value)
{
    restitution = Clamp01(value);
}

// 摩擦係数を設定
void PhysicsBody::SetFriction(float value)
{
    friction = Clamp01(value);
}

#pragma endregion

#pragma region 速度・加速度

// 速度を奪取して物理挙動を停止
Vector3 PhysicsBody::StealVelocity()
{
    // 現在速度を保持
    const Vector3 taken = velocity;

    // 速度と加速度を初期化
    velocity = Vector3(0.0f);
    acceleration = Vector3(0.0f);

    // 物理挙動を停止
    frozen = true;

    // 奪取した速度を返す
    return taken;
}

// 速度を付与して物理挙動を再開
void PhysicsBody::GiveVelocity(const Vector3& value)
{
    // 速度を設定
    velocity = value;

    // 物理挙動を再開
    frozen = false;
}

#pragma endregion

#pragma region 物理挙動制御

// 物理挙動を更新して移動量を取得
Vector3 PhysicsBody::Step(float timeScale)
{
    // 外部制御・停止中・時間停止中は更新しない
    if (mode == MotionMode::Kinematic || frozen || timeScale <= 0.0f)
    {
        acceleration = Vector3(0.0f);
        return Vector3(0.0f);
    }

    // 加速度を速度に反映
    velocity += acceleration * timeScale;

    // 加速度を初期化
    acceleration = Vector3(0.0f);

    // 動的物体に重力を適用
    if (gravityEnabled && (mode == MotionMode::Dynamic || mode == MotionMode::Character))
    {
        velocity.y += gravityPerFrame * timeScale;
        if (maxFallSpeed > 0.0f && velocity.y < -maxFallSpeed) { velocity.y = -maxFallSpeed; }
    }

    // 動的物体に水平方向の減速を適用
    if (mode == MotionMode::Dynamic && horizontalDragPerFrame > 0.0f)
    {
        // 水平方向の速度を取得
        const float speed = std::sqrt(
            velocity.x * velocity.x +
            velocity.z * velocity.z
        );

        // 移動中のみ減速
        if (speed > 0.0f)
        {
            // 減速後の速度を計算
            const float remaining =
                speed - horizontalDragPerFrame * timeScale;

            // 速度が負にならないように倍率を計算
            const float factor =
                remaining > 0.0f ? remaining / speed : 0.0f;

            // 水平方向の速度を減速
            velocity.x *= factor;
            velocity.z *= factor;
        }
    }

    // 時間倍率を適用した移動量を返す
    return velocity * timeScale;
}

// 接触面の法線に応じて速度を補正
void PhysicsBody::ResolveSurfaceVelocity(const Vector3& normal)
{
    // 動的物体以外・停止中は処理しない
    if (mode == MotionMode::Kinematic || frozen) return;

    // 法線ベクトルの長さの二乗を取得
    const float lengthSq =
        normal.x * normal.x +
        normal.y * normal.y +
        normal.z * normal.z;

    // 法線が無効なら処理しない
    if (lengthSq <= 1.0e-8f) return;

    // 法線ベクトルを正規化
    const float invLength = 1.0f / std::sqrt(lengthSq);
    const Vector3 n = normal * invLength;

    // 法線方向の速度成分を取得
    const float incoming =
        velocity.x * n.x +
        velocity.y * n.y +
        velocity.z * n.z;

    // 接触面から離れる方向なら処理しない
    if (incoming >= 0.0f) return;

    // 接触面に向かう速度を反発係数に応じて反射
    const float effectiveRestitution = mode == MotionMode::Character ? 0.0f : restitution;
    velocity -= n * ((1.0f + effectiveRestitution) * incoming);

    // 反射後の法線方向の速度成分を取得
    const float outgoing =
        velocity.x * n.x +
        velocity.y * n.y +
        velocity.z * n.z;

    // 接触面に沿った速度成分を取得
    const Vector3 tangent = velocity - n * outgoing;

    // 摩擦係数に応じて接線方向の速度を減衰
    if (mode == MotionMode::Dynamic) { velocity -= tangent * friction; }
}

// 球体が接地した際、接線速度に対応する角速度へ徐々に近づける
// 衝突点を正確に求める剛体ソルバーではなく、球体専用の簡易転がり処理
void PhysicsBody::ResolveRollingContact(const Vector3& normal, float radius)
{
    if (!rotationEnabled || frozen || mode != MotionMode::Dynamic || radius <= 1.0e-5f) { return; }
    const float lengthSq = normal.LengthSq();
    if (lengthSq <= 1.0e-8f) { return; }
    const Vector3 n = normal / std::sqrt(lengthSq);
    // 床に近い面だけを対象とし、壁への接触で不自然に転がらないようにする
    if (n.y < 0.5f) { return; }

    const Vector3 tangent = velocity - n * velocity.Dot(n);
    const Vector3 desiredAngularVelocity = n.Cross(tangent) / radius;
    // 転がり追従率に応じて目標角速度へ近づける
    const float blend = rollingResponse;
    angularVelocity += (desiredAngularVelocity - angularVelocity) * blend;
}

#pragma endregion

#pragma region 回転物理

void PhysicsBody::SetAngularDragPerFrame(float value)
{
    angularDragPerFrame = value > 0.0f ? value : 0.0f;
}

void PhysicsBody::StabilizeBoxGroundContact(const Vector3& groundNormal)
{
    if (!rotationEnabled || frozen || mode != MotionMode::Dynamic) { return; }
    if (groundNormal.LengthSq() <= 1.0e-8f) { return; }
    const Vector3 n = groundNormal.Normalized();
    // 壁や天井との接触は接地とみなさない
    if (n.y < 0.7f) { return; }

    // 強く移動している間は転倒・押し込み回転を妨げない
    const Vector3 horizontalVelocity(velocity.x, 0.0f, velocity.z);
    if (horizontalVelocity.LengthSq() > 0.25f) { return; }

    const float factor = 1.0f - boxGroundAngularDamping;
    angularVelocity *= factor;
    // 小さい角速度の無限振動を抑える
    if (angularVelocity.LengthSq() < 1.0e-6f) {
        angularVelocity = Vector3(0.0f);
    }
}

bool PhysicsBody::StepRotation(float timeScale, Vector3& axis, float& angle)
{
    axis = Vector3(0.0f);
    angle = 0.0f;
    if (!rotationEnabled || frozen || mode != MotionMode::Dynamic || timeScale <= 0.0f) { return false; }

    const float speed = angularVelocity.Length();
    if (speed <= 1.0e-6f) { angularVelocity = Vector3(0.0f); return false; }

    axis = angularVelocity / speed;
    angle = speed * timeScale;

    // 角速度を線形減衰。移動速度とは独立して扱う
    const float nextSpeed = speed - angularDragPerFrame * timeScale;
    angularVelocity = nextSpeed > 0.0f ? axis * nextSpeed : Vector3(0.0f);
    return true;
}

#pragma endregion

#pragma region 接触による回転応答

void PhysicsBody::SetAngularInertia(float value)
{
    angularInertia = value > 1.0e-6f ? value : 1.0e-6f;
}

void PhysicsBody::ResolveContactAngularVelocity(const Vector3& normal, const Vector3& contactPoint, const Vector3& center, const Vector3& incomingVelocity)
{
    // Characterや物理停止中のActorは回転させない
    if (!rotationEnabled || frozen || mode != MotionMode::Dynamic) { return; }

    const float normalLengthSq = normal.LengthSq();
    if (normalLengthSq <= 1.0e-8f) { return; }
    const Vector3 n = normal / std::sqrt(normalLengthSq);
    const float incomingNormal = incomingVelocity.Dot(n);
    if (incomingNormal >= 0.0f) { return; }

    // 接触点を中心からの相対ベクトルに変換
    const Vector3 lever = contactPoint - center;
    if (lever.LengthSq() <= 1.0e-8f) { return; }

    // 法線方向の衝突インパルスを近似して角速度へ変換
    // 質量・形状別慣性テンソル・接触点速度を含む厳密な剛体解法ではない
    const Vector3 impulse = n * (-(1.0f + restitution) * incomingNormal);
    angularVelocity += lever.Cross(impulse) / angularInertia;
}

#pragma endregion

#pragma region 接触点インパルス

Vector3 PhysicsBody::GetPointVelocity(const Vector3& lever) const
{
    if (mode == MotionMode::Kinematic || frozen) { return Vector3(0.0f); }
    return velocity + (rotationEnabled ? angularVelocity.Cross(lever) : Vector3(0.0f));
}

float PhysicsBody::GetInverseAngularInertia() const
{
    return rotationEnabled && !frozen && mode == MotionMode::Dynamic && angularInertia > 1.0e-6f
        ? 1.0f / angularInertia : 0.0f;
}

void PhysicsBody::ApplyImpulseAtPoint(const Vector3& impulse, const Vector3& lever)
{
    const float invMass = GetInverseMass();
    if (invMass <= 0.0f) { return; }
    velocity += impulse * invMass;
    const float invInertia = GetInverseAngularInertia();
    if (invInertia > 0.0f) { angularVelocity += lever.Cross(impulse) * invInertia; }
}

void PhysicsBody::SetShapeInertiaSphere(float radius)
{
    if (radius <= 0.0f) { return; }
    SetAngularInertia(0.4f * mass * radius * radius);
}

void PhysicsBody::SetShapeInertiaBox(const Vector3& size)
{
    // 等方近似：各軸の慣性モーメントの平均
    const float sum = size.x * size.x + size.y * size.y + size.z * size.z;
    SetAngularInertia(mass * sum / 18.0f);
}

void PhysicsBody::SetShapeInertiaCapsule(float radius, float segmentLength)
{
    // 円柱と球の複合形状の簡易近似
    const float length = segmentLength > 0.0f ? segmentLength : 0.0f;
    SetAngularInertia(mass * (0.4f * radius * radius + length * length / 12.0f));
}

#pragma endregion
