#pragma once

#include "../../../Common/Vector3.h"
#include "../../../Common/Quaternion.h"

#include "../../../Manager/TimeScale/TimeScale.h"

// 物体の速度・加速度・重力・摩擦・回転などの物理挙動を管理するクラス。
// Actorの位置や回転を直接保持するのではなく、
// 物理演算によって発生する移動量や回転量を計算する。
class PhysicsBody
{
public:

    // 物体の移動方法を指定するための挙動モード。
    enum class MotionMode {

        // プレイヤーなど、入力によって移動を制御する物体。
        // 物理的な速度を持ちながら、キャラクター操作を優先する。
        Character,

        // 重力・衝突・摩擦などの物理演算によって移動する物体。
        // 箱や球体など、自然な物理挙動を行うオブジェクトに使用する。
        Dynamic,

        // アニメーションや外部処理によって移動を制御する物体。
        // 衝突によるインパルスでは動かないものとして扱う。
        Kinematic
    };

    // コンストラクタ。
    // 各物理パラメータはメンバ変数に設定された初期値を使用する。
    PhysicsBody() = default;

#pragma region 挙動モード

    // 物体の挙動モードを設定する。
    // Character / Dynamic / Kinematic の切り替えに使用する。
    void SetMode(MotionMode value) { mode = value; }

    // 現在設定されている挙動モードを取得する。
    MotionMode GetMode() const { return mode; }

#pragma endregion

#pragma region 速度・加速度

    // 現在の移動速度を指定したベクトルで上書きする。
    // X・Y・Zそれぞれの方向と速度を直接指定できる。
    // ゼロではない速度を設定した場合はスリープ状態を解除する。
    void SetVelocity(const Vector3& value) { velocity = value; if (value.LengthSq() > 1.0e-8f) WakeUp(); }

    // 現在の移動速度を取得する。
    // 戻り値は内部速度へのconst参照であり、直接書き換えることはできない。
    const Vector3& GetVelocity() const { return velocity; }

    // 現在の移動速度に指定したベクトルを加算する。
    // 瞬間的な加速やジャンプ、ノックバックなどに使用できる。
    // ゼロではない加算量を指定した場合はスリープ状態を解除する。
    void AddVelocity(const Vector3& delta) { if (delta.LengthSq() > 1.0e-8f) WakeUp(); velocity += delta; }

    // 加速度を蓄積する。
    // AddVelocityとは異なり、速度をその場で直接変更せず、
    // 物理更新時に速度へ反映するための加速度を加算する。
    // 加速度を追加した場合はスリープ状態を解除する。
    void AddAcceleration(const Vector3& value) { if (value.LengthSq() > 1.0e-8f) WakeUp(); acceleration += value; }

    // 現在の速度を奪い取り、その速度ベクトルを返す。
    // 勢いの奪取など、物体の運動を別の対象へ移す処理に使用する。
    Vector3 StealVelocity();

    // 指定した速度ベクトルを物体へ付与する。
    // 奪取した勢いの受け渡しなどに使用する。
    void GiveVelocity(const Vector3& value);

#pragma endregion

#pragma region 物理パラメータ

    // 重力による加速を有効・無効にする。
    // trueで重力を適用し、falseで重力による速度変化を停止する。
    void SetGravityEnabled(bool value) { gravityEnabled = value; }

    // 1フレームあたりに適用する重力加速度を設定する。
    // 通常はY軸の負方向へ加速させるため、負の値を指定する。
    void SetGravityPerFrame(float value) { gravityPerFrame = value; }

    // 現在有効な重力加速度を取得する。
    // 重力が無効な場合は0を返す。
    // Boxの接地時に発生する回転計算などから参照する。
    float GetGravityPerFrame() const { return gravityEnabled ? gravityPerFrame : 0.0f; }

    // 下方向への最大落下速度を設定する。
    // 正の値で落下速度の上限を指定し、負の値は0として扱う。
    // 重力によって落下速度が過剰に大きくなることを防ぐ。
    void SetMaxFallSpeed(float value) { maxFallSpeed = value > 0.0f ? value : 0.0f; }

    // 現在設定されている最大落下速度を取得する。
    float GetMaxFallSpeed() const { return maxFallSpeed; }

    // 1フレームあたりの水平方向の速度減衰率を設定する。
    // X・Z方向の速度を徐々に減らし、移動を自然に停止させるために使用する。
    // 接触による摩擦とは異なり、速度そのものに作用する減衰処理。
    void SetHorizontalDragPerFrame(float value);

    // 衝突時の反発係数を設定する。
    // 値が大きいほど衝突後に跳ね返りやすくなる。
    // 例えば、ボールが床に衝突した際の跳ね返りに影響する。
    void SetRestitution(float value);

    // 物体の摩擦係数を設定する。
    // 静止摩擦・動摩擦をまとめて設定するための共通インターフェース。
    void SetFriction(float value);

    // 現在設定されている反発係数を取得する。
    float GetRestitution() const { return restitution; }

    // 現在設定されている動摩擦係数を取得する。
    // 互換性のための共通摩擦取得関数。
    float GetFriction() const { return dynamicFriction; }

    // 静止摩擦係数を設定する。
    // 接触している物体が滑り始めるのを抑えるための摩擦。
    // 値が大きいほど、静止状態を維持しやすくなる。
    // 係数は1.0を超える値も指定できる。
    void SetStaticFriction(float value);

    // 動摩擦係数を設定する。
    // 接触面上を滑っている物体の相対的な滑りを抑える摩擦。
    // 値が大きいほど、滑走中の速度が失われやすくなる。
    // 係数は1.0を超える値も指定できる。
    void SetDynamicFriction(float value);

    // 現在設定されている静止摩擦係数を取得する。
    float GetStaticFriction() const { return staticFriction; }

    // 現在設定されている動摩擦係数を取得する。
    float GetDynamicFriction() const { return dynamicFriction; }

    // 転がり抵抗を設定する。
    // 球体などが接触面上を転がる際、回転がいつまでも続くことを抑える。
    // 通常の摩擦や角速度減衰とは別の物理パラメータ。
    void SetRollingResistance(float value);

    // 現在設定されている転がり抵抗を取得する。
    float GetRollingResistance() const { return rollingResistance; }

    // 接触面の法線を使用して転がり抵抗を適用する。
    // 接触によって支持されている間の回転運動を減衰させる。
    // normalには接触面の法線ベクトルを指定する。
    void ApplyContactRollingResistance(const Vector3& normal);

    // 物体の質量を設定する。
    // 衝突時の速度変化やインパルス計算に使用する。
    // 質量が大きいほど、同じ衝突インパルスによる速度変化は小さくなる。
    void SetMass(float value);

    // 現在設定されている質量を取得する。
    float GetMass() const { return mass; }

    // 質量の逆数を取得する。
    // 衝突計算で使用し、通常は1.0f / massを返す。
    // フリーズ中またはKinematicの場合は0を返し、
    // 衝突インパルスによる移動速度の変化を受けないものとして扱う。
    float GetInverseMass() const { return (frozen || mode == MotionMode::Kinematic) ? 0.0f : 1.0f / mass; }

#pragma endregion

#pragma region 物理挙動制御

    // 物理挙動の停止・再開を設定する。
    // trueでフリーズ状態にし、falseでフリーズを解除する。
    // 解除時にはスリープ状態も解除する。
    // 速度や加速度の値自体を初期化する処理ではない。
    void SetFrozen(bool value) { frozen = value; if (!value) WakeUp(); }

    // 現在フリーズ状態かどうかを取得する。
    bool IsFrozen() const { return frozen; }

    // 物理演算による自動スリープを有効・無効にする。
    // 主にDynamic物体が接地して十分静止した場合にスリープする。
    // falseを指定すると自動スリープを無効化し、現在のスリープも解除する。
    void SetSleepEnabled(bool value) { sleepEnabled = value; if (!value) WakeUp(); }

    // 自動スリープが有効かどうかを取得する。
    bool IsSleepEnabled() const { return sleepEnabled; }

    // 現在スリープ状態かどうかを取得する。
    // スリープは静止した物体の不要な物理計算を減らすための状態。
    // 明示的に挙動を停止するフリーズとは異なる。
    bool IsSleeping() const { return sleeping; }

    // スリープ状態を解除して物理演算を再開できる状態にする。
    // 外力や速度の変更が発生した際に使用する。
    void WakeUp();

    // 衝突拘束の解決後に、スリープ状態を更新する。
    // supportedには床などによって支持されているかを指定する。
    // movingSupportには支持している物体が動いているかを指定する。
    // 接地状態や運動量を評価し、スリープへの移行・解除を判断する。
    void UpdateSleepState(bool supported, bool movingSupport);

    // 1フレーム分の並進方向の物理挙動を更新する。
    // 速度・加速度・重力・減衰などを使用して移動量を計算する。
    // timeScaleには時間倍率を指定する。
    // 戻り値はActorの座標更新に使用する移動ベクトル。
    Vector3 Step(float timeScale = TimeScale::Get());

    // 物体の角速度を設定する。
    // ワールド座標系の各軸に対する回転速度をラジアン/フレームで指定する。
    // ゼロではない角速度を設定した場合はスリープ状態を解除する。
    void SetAngularVelocity(const Vector3& value) { angularVelocity = value; if (value.LengthSq() > 1.0e-8f) WakeUp(); }

    // 現在の角速度を取得する。
    // 単位はラジアン/フレーム。
    const Vector3& GetAngularVelocity() const { return angularVelocity; }

    // 現在の角速度に指定したベクトルを加算する。
    // 衝突によって物体を回転させる場合などに使用する。
    void AddAngularVelocity(const Vector3& value) { if (value.LengthSq() > 1.0e-8f) WakeUp(); angularVelocity += value; }

    // 1フレームあたりの角速度減衰量を設定する。
    // 回転速度を徐々に減らし、回転が長時間続くことを抑える。
    // 接触による転がり抵抗とは異なる。
    void SetAngularDragPerFrame(float value);

    // Boxが地面に接触しているときの回転減衰率を設定する。
    // 0.0～1.0の範囲に制限される。
    // 球体には適用せず、Boxの接地時の回転安定化に使用する。
    void SetBoxGroundAngularDamping(float value) { boxGroundAngularDamping = Clamp01(value); }

    // 現在設定されているBox接地時の回転減衰率を取得する。
    float GetBoxGroundAngularDamping() const { return boxGroundAngularDamping; }

    // Boxが安定した床に接触している場合に回転を減衰させる。
    // groundNormalには接触面の法線ベクトルを指定する。
    // 接地したBoxが不自然に回転し続けることを防ぐ。
    void StabilizeBoxGroundContact(const Vector3& groundNormal);

    // 物理演算による回転の有効・無効を設定する。
    // falseの場合は物理回転を無効化する。
    // プレイヤーなど、向きを操作で制御する物体では通常falseを使用する。
    void SetRotationEnabled(bool value) { rotationEnabled = value; }

    // 物理演算による回転が有効かどうかを取得する。
    bool IsRotationEnabled() const { return rotationEnabled; }

    // 1フレーム分の回転量を計算する。
    // timeScaleには時間倍率を指定する。
    // axisには回転軸、angleには回転角度を出力する。
    // 戻り値は回転更新が必要な場合にtrueとなる。
    bool StepRotation(Vector3& axis, float& angle, float timeScale = TimeScale::Get());

    // 物体の回転慣性を近似値で設定する。
    // 値が大きいほど、衝突インパルスによって回転しにくくなる。
    // 物体の質量とは別に、回転のしにくさを表すパラメータ。
    void SetAngularInertia(float value);

    // 現在設定されている回転慣性の代表値を取得する。
    float GetAngularInertia() const { return angularInertia; }

    // 衝突点の位置と衝突方向から角速度を更新する。
    // normalには衝突面の法線、contactPointには接触点のワールド座標、
    // centerには物体中心のワールド座標を指定する。
    // incomingVelocityには衝突時の速度を指定する。
    // 衝突によって物体が回転する挙動を近似的に計算する。
    void ResolveContactAngularVelocity(const Vector3& normal, const Vector3& contactPoint, const Vector3& center, const Vector3& incomingVelocity);

    // 物体上の指定した点の速度を取得する。
    // 物体全体の並進速度と、回転によって発生する速度を合成する。
    // leverには物体中心から対象点までの相対位置ベクトルを指定する。
    Vector3 GetPointVelocity(const Vector3& lever) const;

    // 指定した接触点にインパルスを適用する。
    // impulseには加えるインパルス、leverには中心から接触点への相対位置を指定する。
    // 並進速度と角速度の両方に反映する。
    void ApplyImpulseAtPoint(const Vector3& impulse, const Vector3& lever);

    // 球体形状に対応する慣性モーメントを設定する。
    // radiusには球体の半径を指定する。
    // 球体の質量分布を近似し、衝突時の回転計算に使用する。
    void SetShapeInertiaSphere(float radius);

    // Box形状に対応する慣性モーメントを設定する。
    // sizeにはBoxの各軸方向の大きさを指定する。
    // 形状による回転のしにくさを各軸ごとに計算する。
    void SetShapeInertiaBox(const Vector3& size);

    // カプセル形状に対応する慣性モーメントを設定する。
    // radiusにはカプセルの半径、
    // segmentLengthにはカプセル中央の線分長を指定する。
    void SetShapeInertiaCapsule(float radius, float segmentLength);

    // 代表的な逆慣性モーメントを取得する。
    // 衝突時の回転速度変化を計算するために使用する。
    // 慣性モーメントが大きいほど逆数は小さくなる。
    float GetInverseAngularInertia() const;

    // 剛体ローカル座標系の各軸に対応する慣性モーメントを取得する。
    // X・Y・Z方向それぞれの回転のしにくさを表す。
    const Vector3& GetLocalInertia() const { return localInertia; }

    // 指定したワールドベクトルに逆慣性テンソルを適用する。
    // worldRotationには物体の現在のワールド回転を指定する。
    // ローカル座標系の慣性モーメントを考慮して、
    // ワールド座標系での角速度変化を求めるために使用する。
    Vector3 MultiplyInverseInertia(const Vector3& worldVector, const Quaternion& worldRotation) const;

    // 物体のワールド回転を考慮して接触点へインパルスを適用する。
    // impulseにはインパルス、leverには物体中心から接触点への相対位置、
    // worldRotationには物体のワールド回転を指定する。
    // 並進速度と角速度の両方を更新する。
    void ApplyImpulseAtPoint(const Vector3& impulse, const Vector3& lever, const Quaternion& worldRotation);

    // 接触面の法線方向に応じて物体の速度を補正する。
    // 壁や床などの接触面に対する速度の処理に使用する。
    void ResolveSurfaceVelocity(const Vector3& normal);

#pragma endregion

private:

    // 指定した値を0.0f～1.0fの範囲に制限する。
    // 減衰率など、一定範囲に収める必要があるパラメータに使用する。
    static float Clamp01(float value);

#pragma region 物理状態

    // 現在の挙動モード。
    // 初期状態は物理演算によって移動するDynamic。
    MotionMode mode = MotionMode::Dynamic;

    // 現在の並進速度。
    // ワールド座標系のX・Y・Z方向の速度を保持する。
    Vector3 velocity = Vector3(0.0f);

    // 現在蓄積されている加速度。
    // AddAccelerationによって加算され、物理更新で使用される。
    Vector3 acceleration = Vector3(0.0f);

    // 現在の角速度。
    // ワールド座標系の各軸に対する回転速度を保持する。
    Vector3 angularVelocity = Vector3(0.0f);

    // 1フレームあたりの角速度減衰量。
    // 回転運動を徐々に弱めるために使用する。
    float angularDragPerFrame = 0.0f;

    // Boxが床に接触しているときの回転減衰率。
    // 初期値は0.08。
    float boxGroundAngularDamping = 0.08f;

    // 旧APIとの互換性のために保持する回転慣性の代表値。
    // 初期値は1.0。
    float angularInertia = 1.0f;

    // 単位質量あたりの各軸の慣性モーメント。
    // 質量が変化した場合に慣性モーメントを再計算するために使用する。
    Vector3 inertiaPerMass = Vector3(1.0f);

    // 現在の各軸方向の慣性モーメント。
    // 物体の形状・質量による回転のしにくさを保持する。
    Vector3 localInertia = Vector3(1.0f);

    // 現在の質量と形状設定から慣性モーメントを再計算する。
    // 質量や形状パラメータが変更された場合に内部で使用する。
    void RebuildInertia();

    // 物理演算による回転が有効かどうか。
    // 初期状態はfalseで、意図しない回転を防ぐ。
    bool rotationEnabled = false;

    // 重力を適用するかどうか。
    // 初期状態はtrue。
    bool gravityEnabled = true;

    // 物理挙動を明示的に停止しているかどうか。
    // 初期状態はfalse。
    bool frozen = false;

    // 自動スリープを有効にするかどうか。
    // 初期状態はtrue。
    bool sleepEnabled = true;

    // 現在スリープ状態かどうか。
    // 初期状態はfalse。
    bool sleeping = false;

    // スリープ移行判定に使用する連続静止フレーム数。
    // 一定期間静止したかを評価するために使用する。
    int sleepFrames = 0;

#pragma endregion

#pragma region 物理パラメータ

    // 1フレームあたりの重力加速度。
    // 初期値は-0.9で、Y軸の負方向に作用する。
    float gravityPerFrame = -0.9f;

    // 下方向への最大落下速度。
    // 初期値は100.0。
    float maxFallSpeed = 100.0f;

    // 1フレームあたりの水平方向の速度減衰率。
    // 初期値は0.05。
    float horizontalDragPerFrame = 0.05f;

    // 衝突時の反発係数。
    // 初期値は0.25。
    float restitution = 0.25f;

    // 旧APIとの互換性のために保持する摩擦係数。
    // SetFrictionなどの設定関数によって同期される。
    float friction = 0.5f;

    // 静止摩擦係数。
    // 物体が滑り始めるのを抑えるために使用する。
    float staticFriction = 0.65f;

    // 動摩擦係数。
    // 物体が滑っている間の摩擦に使用する。
    float dynamicFriction = 0.5f;

    // 転がり抵抗。
    // 接地中の転がり運動を減衰させる。
    float rollingResistance = 0.01f;

    // 衝突応答に使用する物体の質量。
    // 初期値は1.0。
    float mass = 1.0f;

#pragma endregion
};
