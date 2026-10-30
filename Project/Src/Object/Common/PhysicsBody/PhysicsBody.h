
#pragma once

#include "../../../Common/Vector3.h"

// 物理挙動制御
class PhysicsBody
{
public:

    // 挙動モード
    enum class MotionMode {

        // キャラクター操作用
        Character,

        // 物理演算による移動
        Dynamic,

        // 外部から移動を制御
        Kinematic
    };

    // コンストラクタ
    PhysicsBody() = default;

#pragma region 挙動モード

    // 挙動モードを設定
    void SetMode(MotionMode value) { mode = value; }

    // 挙動モードを取得
    MotionMode GetMode() const { return mode; }

#pragma endregion

#pragma region 速度・加速度

    // 速度を設定
    void SetVelocity(const Vector3& value) { velocity = value; }

    // 速度を取得
    const Vector3& GetVelocity() const { return velocity; }

    // 速度を加算
    void AddVelocity(const Vector3& delta) { velocity += delta; }

    // 加速度を加算
    void AddAcceleration(const Vector3& value) { acceleration += value; }

    // 速度を奪取して停止
    Vector3 StealVelocity();

    // 奪取した速度を付与
    void GiveVelocity(const Vector3& value);

#pragma endregion

#pragma region 物理パラメータ

    // 重力の有効・無効を設定
    void SetGravityEnabled(bool value) { gravityEnabled = value; }

    // 1フレームあたりの重力加速度を設定
    void SetGravityPerFrame(float value) { gravityPerFrame = value; }

    // Boxの接地回転で重力加速度を参照
    float GetGravityPerFrame() const { return gravityEnabled ? gravityPerFrame : 0.0f; }

    // 最大落下速度（正の値、単位は距離/フレーム）
    void SetMaxFallSpeed(float value) { maxFallSpeed = value > 0.0f ? value : 0.0f; }
    float GetMaxFallSpeed() const { return maxFallSpeed; }

    // 1フレームあたりの水平方向の減衰率を設定
    void SetHorizontalDragPerFrame(float value);

    // 反発係数を設定
    void SetRestitution(float value);

    // 摩擦係数を設定
    void SetFriction(float value);

    // 反発係数を取得
    float GetRestitution() const { return restitution; }

    // 摩擦係数を取得
    float GetFriction() const { return friction; }

    // 質量（0より大きい値）。衝突時の速度変化に使用
    void SetMass(float value) { mass = value > 0.0001f ? value : 0.0001f; }
    float GetMass() const { return mass; }
    float GetInverseMass() const { return (frozen || mode == MotionMode::Kinematic) ? 0.0f : 1.0f / mass; }

    // 接地時に目標角速度へ近づく割合（0.0f～1.0f）
    void SetRollingResponse(float value) { rollingResponse = Clamp01(value); }
    float GetRollingResponse() const { return rollingResponse; }

#pragma endregion

#pragma region 物理挙動制御

    // 物理挙動の停止・再開を設定
    void SetFrozen(bool value) { frozen = value; }

    // 物理挙動が停止中か取得
    bool IsFrozen() const { return frozen; }

    // 物理挙動を更新して移動量を取得
    Vector3 Step(float timeScale);

    // 角速度（ワールド軸・ラジアン/フレーム）
    void SetAngularVelocity(const Vector3& value) { angularVelocity = value; }
    const Vector3& GetAngularVelocity() const { return angularVelocity; }
    void AddAngularVelocity(const Vector3& value) { angularVelocity += value; }

    // 角速度の減衰量（ラジアン/フレーム）
    void SetAngularDragPerFrame(float value);

    // Box接地中の回転減衰率（0～1、1接触あたり）。球体には適用しない
    void SetBoxGroundAngularDamping(float value) { boxGroundAngularDamping = Clamp01(value); }
    float GetBoxGroundAngularDamping() const { return boxGroundAngularDamping; }

    // Boxが安定した床に接触したときのみ回転を減衰
    void StabilizeBoxGroundContact(const Vector3& groundNormal);

    void SetRotationEnabled(bool value) { rotationEnabled = value; }
    bool IsRotationEnabled() const { return rotationEnabled; }

    // 回転更新量を軸角に変換して返す。角速度自体はワールド座標系。
    bool StepRotation(float timeScale, Vector3& axis, float& angle);

    // 回転慣性の近似値（大きいほど衝突で回転しにくい）
    void SetAngularInertia(float value);
    float GetAngularInertia() const { return angularInertia; }

    // 接触点と法線から、衝突時の角速度を近似的に更新する
    // contactPoint / center はワールド座標
    void ResolveContactAngularVelocity(const Vector3& normal, const Vector3& contactPoint, const Vector3& center, const Vector3& incomingVelocity);

    // 接触面の法線に応じて速度を補正
    // 接触点での速度（並進速度 + 角速度による速度）
    Vector3 GetPointVelocity(const Vector3& lever) const;

    // 接触点に作用するインパルスを並進・回転へ反映
    void ApplyImpulseAtPoint(const Vector3& impulse, const Vector3& lever);

    // 接触点のインパルス計算用の逆慣性（球・箱・カプセルの近似）
    void SetShapeInertiaSphere(float radius);
    void SetShapeInertiaBox(const Vector3& size);
    void SetShapeInertiaCapsule(float radius, float segmentLength);
    float GetInverseAngularInertia() const;

    void ResolveSurfaceVelocity(const Vector3& normal);

    // 球体の接地面に対する滑りを角速度へ近づける（簡易転がり）
    void ResolveRollingContact(const Vector3& normal, float radius);

#pragma endregion

private:

    // 0.0f～1.0fの範囲に制限
    static float Clamp01(float value);

#pragma region 物理状態

    // 挙動モード
    MotionMode mode = MotionMode::Dynamic;

    // 現在速度
    Vector3 velocity = Vector3(0.0f);

    // 加速度
    Vector3 acceleration = Vector3(0.0f);

    // 角速度はワールド座標系
    Vector3 angularVelocity = Vector3(0.0f);
    float angularDragPerFrame = 0.0f;
    float boxGroundAngularDamping = 0.08f;
    float angularInertia = 1.0f;
    // 既存Actorの意図しない回転を防ぐため明示的に有効化する
    bool rotationEnabled = false;

    // 重力を適用するか
    bool gravityEnabled = true;

    // 物理挙動を停止しているか
    bool frozen = false;

#pragma endregion

#pragma region 物理パラメータ

    // 1フレームあたりの重力加速度
    float gravityPerFrame = -0.9f;
    float maxFallSpeed = 100.0f;

    // 1フレームあたりの水平方向の減衰率
    float horizontalDragPerFrame = 0.0f;

    // 反発係数
    float restitution = 0.25f;

    // 摩擦係数
    float friction = 0.5f;

    // 転がりの追従率（摩擦とは独立）
    float rollingResponse = 0.125f;

    // 衝突応答に使用する質量
    float mass = 1.0f;

#pragma endregion
};