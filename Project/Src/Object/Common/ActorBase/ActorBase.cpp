#include "ActorBase.h"

#include <stdexcept>

#include "../../../Application/Application.h"

#include "../../../Common/Vector2.h"
#include "../../../Common/Vector2I.h"

#include "../../../Manager/TimeScale/TimeScale.h"

#include "../../../Manager/Collision/CurrentCollisionManager.h"

#include "../PhysicsBody/PhysicsBody.h"

#include "../Collider/ColliderBase.h"

#include "../Shader/ShaderBase.h"
#include "../Shader/RimLightShader.h"

#include "../DataLoad/ParameterLoad.h"

ActorBase::ActorBase() :
	trans(),
	physicsBody(nullptr),
	colliders(),

	colliderMobility(COLLIDER_MOBILITY::Movable),
	gravityFlg(false),

	pushFlg(true),
	pushWeight(0),


	isGroundMaster(false),

	isDraw(true),
	drawType(ACTOR_DRAW_TYPE::Default),

	shader(nullptr),

	parameter(nullptr),

	childActors()
{
}

ActorBase::ActorBase(const std::string& parameterPath) :
	trans(),
	physicsBody(nullptr),
	colliders(),

	colliderMobility(COLLIDER_MOBILITY::Movable),
	gravityFlg(false),

	pushFlg(true),
	pushWeight(0),


	isGroundMaster(false),

	isDraw(true),
	drawType(ACTOR_DRAW_TYPE::Default),

	shader(nullptr),

	parameter(new ParameterLoad(parameterPath)),

	childActors()
{
}

void ActorBase::Init(void)
{
	// デフォルトのシェーダーを生成
	DefaultCreateShader();

	// 最終派生固有
	SubInit();

	// 中間基底固有
	BaseInit();

	// 子アクターの初期化
	for (ActorBase* child : childActors) { child->Init(); }

	// モデル制御情報の適用
	trans.Attach();

	// コライダーの初期化
	for (ColliderBase* collider : colliders) { collider->Init(); }
	
	// パラメータの解放
	if (parameter != nullptr) {
		parameter->Release();
		delete parameter;
		parameter = nullptr;
	}
}

void ActorBase::Update(void)
{
	// CCDのため、物理演算の有無に関係なく前フレーム座標を記録
	trans.prevPos = trans.pos;

	// 最終派生固有
	SubUpdate();

	// 中間基底固有
	BaseUpdate();

	// 物理挙動を持つActorのみ物理更新
	if (physicsBody != nullptr) {
        const float timeScale = TimeScale::Get();
        trans.pos += physicsBody->Step(timeScale);

        // 回転物理を有効化したActorだけ、角速度を回転へ反映
        Vector3 rotationAxis(0.0f);
        float rotationAngle = 0.0f;
        if (physicsBody->StepRotation(timeScale, rotationAxis, rotationAngle)) {
            // ワールド軸の回転を既存のQuaternionへ左から合成
            trans.SetRotation(Quaternion::FromAxisAngle(rotationAxis, rotationAngle) * trans.rotation);
        }
    }

	// 次の衝突判定で接地情報を再設定
	isGroundMaster = false;

	// 子アクターの更新
	for (ActorBase* child : childActors) { child->Update(); }

	// シェーダーの更新
	if (shader != nullptr) { shader->Update(); }
}

void ActorBase::Draw(void)
{
	// 描画判定
	if (!isDraw) { return; }

	// シェーダーの適用
	if (shader != nullptr) { shader->Apply(); }

	// 最終派生固有
	SubDraw();

	// 中間基底固有
	BaseDraw();

	// モデルの描画
	trans.Draw();

	// シェーダーのリセット
	if (shader != nullptr) { 
		shader->ResetApply();

		// ついでにシェーダーの終了判定
		if (shader->IsEnd()) {
			// シェーダーを消去
			ResetShader();

			// デフォルトのシェーダーを生成
			DefaultCreateShader();
		}
	}
}

void ActorBase::Release(void)
{
	// 最終派生固有
	SubRelease();

	// 中間基底固有
	BaseRelease();

	// 子アクターの解放
	for(ActorBase*& child : childActors) {
		if (!child) { continue; }
		child->Release();
		delete child;
		child = nullptr;
	}

	// シェーダーの解放
	ResetShader();

	// パラメータの解放
	if (parameter != nullptr) {
		parameter->Release();
		delete parameter;
		parameter = nullptr;
	}

	// 物理挙動を解放
	delete physicsBody;
	physicsBody = nullptr;

	// 当たり判定情報を解放
	for (ColliderBase*& collider : colliders) {
		if (!collider) { continue; }
		delete collider;
		collider = nullptr;
	}
	colliders.clear();

	// モデル制御情報の解放
	trans.Release();
}

std::vector<ColliderBase*> ActorBase::GetColliders(void) const
{
	std::vector<ColliderBase*> ret = {};

	for (ColliderBase* collider : colliders) { ret.emplace_back(collider); }

	for (ActorBase* child : childActors) {
		for (ColliderBase* collider : child->GetColliders()) { ret.emplace_back(collider); }
	}

	return ret;
}

void ActorBase::DrawColliderDebug(void) const
{
	for (ColliderBase* collider : GetColliders()) {
		if (collider->GetJudgeFlg()) collider->DrawDebug(); 
	}
}

Vector3 ActorBase::VelSteal(void)
{
    if (physicsBody == nullptr) { return Vector3(0.0f); }
    const Vector3 taken = physicsBody->StealVelocity();
    // Collider区分は維持
    
    return taken;
}

void ActorBase::VelGive(const Vector3& vel)
{
    if (physicsBody == nullptr) { CreatePhysicsBody(); }
    physicsBody->GiveVelocity(vel);
    // PhysicsBodyが再開
}

void ActorBase::OnTargeted()
{
	// リムライトをつける
	CreateShader(new RimLightShader(SHADER_VERTEX_TYPE::Frame1));
}

void ActorBase::OnUntargeted()
{
	// 現在のシェーダー破棄
	ResetShader();
	// デフォルトシェーダーに戻す
	DefaultCreateShader();
}

void ActorBase::SetColliderMobility(COLLIDER_MOBILITY value)
{
	colliderMobility = value;
	trans.dynamicFlg = (value == COLLIDER_MOBILITY::Movable);
	for (ColliderBase* collider : colliders) {
		if (collider != nullptr) { collider->SetMobility(value); }
	}
	// チャンク区分変更時だけ静的・動的チャンクを再構築
	// （初期化前に呼ばれる可能性があるため、登録済みColliderがある場合のみ）
	// 登録タイミングが不明なため、ここでは再構築を呼ばず、Manager側の明示更新に委ねる。
}

void ActorBase::SetGravityFlg(bool flg)
{
	gravityFlg = flg;
	if (flg && physicsBody == nullptr) { CreatePhysicsBody(); }
	if (physicsBody != nullptr) { physicsBody->SetGravityEnabled(flg); }
}

void ActorBase::SetDynamicFlg(bool flg)
{
	// 旧API互換: 物理挙動ではなくチャンク登録区分のみ変更
	SetColliderMobility(flg ? COLLIDER_MOBILITY::Movable : COLLIDER_MOBILITY::Static);
}

void ActorBase::CreatePhysicsBody(void)
{
	if (physicsBody != nullptr) { return; }
	physicsBody = new PhysicsBody();
	physicsBody->SetGravityEnabled(gravityFlg);
	physicsBody->SetGravityPerFrame(GRAVITY);
}

void ActorBase::AddCollider(ColliderBase* newCollider)
{
	newCollider->SetActorPtr(this);
	newCollider->SetMobility(colliderMobility);
	newCollider->SetOnCollisionFunc([this](COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result) { this->OnCollision(ownTag, other, result); });
	newCollider->SetOnGroundedFunc([this](COLLIDER_TAG ownTag, const ColliderBase& other, const Vector3& pushVec) { this->OnPushed(ownTag, other, pushVec); });
	colliders.emplace_back(newCollider);
}

void ActorBase::CreateShader(ShaderBase* newShader)
{
	// 既に生成されている場合、破棄
	ResetShader();

	// 新規シェーダーをこのアクターに適用
	shader = newShader;
	// 初期化
	shader->Init();
}

void ActorBase::ResetShader()
{
	if (shader != nullptr) {
		shader->Release();
		delete shader;
		shader = nullptr;
	}
}

#pragma region パラメーター外部ファイル管理に関する関数

bool ActorBase::IsParameterExist(const std::string& fileName, const std::string& parameterName)const
{
	if (parameter == nullptr) { throw std::runtime_error("ParameterLoadクラスが生成されていません"); }
	return parameter->IsParameterExist(fileName, parameterName);
}

float ActorBase::GetParameter(const std::string& fileName, const std::string& parameterName, int index)const
{
	if (parameter == nullptr) { throw std::runtime_error("ParameterLoadクラスが生成されていません"); }
	return parameter->GetParameter(fileName, parameterName, index);
}

const std::vector<float>& ActorBase::GetParameterArray(const std::string& fileName, const std::string& parameterName)const
{
	if (parameter == nullptr) { throw std::runtime_error("ParameterLoadクラスが生成されていません"); }
	return parameter->GetParameterArray(fileName, parameterName);
}

int ActorBase::GetParameterToInt(const std::string& fileName, const std::string& parameterName, int index)const
{
	if (parameter == nullptr) { throw std::runtime_error("ParameterLoadクラスが生成されていません"); }
	return parameter->GetParameterToInt(fileName, parameterName, index);
}

Vector3 ActorBase::GetParameterToVector3(const std::string& fileName, const std::string& parameterName)
{
	if (parameter == nullptr) { throw std::runtime_error("ParameterLoadクラスが生成されていません"); }
	return parameter->GetParameterToVector3(fileName, parameterName);
}

Vector2 ActorBase::GetParameterToVector2(const std::string& fileName, const std::string& parameterName)
{
	if (parameter == nullptr) { throw std::runtime_error("ParameterLoadクラスが生成されていません"); }
	return parameter->GetParameterToVector2(fileName, parameterName);
}

Vector2I ActorBase::GetParameterToVector2I(const std::string& fileName, const std::string& parameterName)
{
	if (parameter == nullptr) { throw std::runtime_error("ParameterLoadクラスが生成されていません"); }
	return parameter->GetParameterToVector2I(fileName, parameterName);
}

#pragma endregion

bool ActorBase::GetJudgeFlg(void)
{
	for (ColliderBase* collider : colliders) {
		if (!collider) { continue; }
		if (collider->GetJudgeFlg()) { return true; }
	}
	return false;
}

void ActorBase::SetJudgeFlg(bool flg)
{
	for (ColliderBase* collider : colliders) {
		if (!collider) { continue; }
		collider->SetJudgeFlg(flg);
	}
}

void ActorBase::MoveAccel(const Vector3& vec)
{
	if (vec == 0.0f) { return; }
	if (physicsBody == nullptr) { CreatePhysicsBody(); }

	// 入力量に応じて加速
	if (physicsBody != nullptr && !physicsBody->IsFrozen()) {
        const Vector3 v = physicsBody->GetVelocity();
        const float speedXZ = sqrtf(v.x * v.x + v.z * v.z);
        const float steer = speedXZ > ACCEL_MAX ? 0.2f : 1.0f;
        physicsBody->AddVelocity((vec * ACCEL_RATE * steer) * TimeScale::Get());
        // 通常速度だけを制限。付与された高速の勢いは維持する
        if (speedXZ <= ACCEL_MAX) {
            Vector3 next = physicsBody->GetVelocity();
            const float nextXZ = sqrtf(next.x * next.x + next.z * next.z);
            if (nextXZ > ACCEL_MAX && nextXZ > 0.0f) {
                const float rate = ACCEL_MAX / nextXZ;
                next.x *= rate; next.z *= rate;
                physicsBody->SetVelocity(next);
            }
        }
    }

	// 移動方向から目標のY軸回転を取得
	const float targetAngle = atan2f(vec.x, vec.z);

	// 目標回転
	const Quaternion targetRotation = Quaternion::FromRotationY(targetAngle);

	// 現在の回転から目標回転へ補間
	trans.SetRotation(
		Quaternion::Slerp(
			trans.rotation,
			targetRotation,
			0.25f * TimeScale::Get()
		)
	);
}

void ActorBase::ReflexiveBehavior(Vector3& velocity, const Vector3& pushVec)
{
	if (pushVec.Length() <= 0.0001f) { return; }

	// 押し出し方向
	Vector3 normal = pushVec.Normalized();

	// 押し出し方向に対する速度成分
	float dot = velocity.Dot(normal);

	// すでに離れる方向へ移動しているなら反射しない
	if (dot >= 0.0f) { return; }

	// 反発係数
	constexpr float restitution = 0.75f;

	// 衝突面に向かう速度成分だけを反射
	velocity -= normal * ((1.0f + restitution) * dot);
}