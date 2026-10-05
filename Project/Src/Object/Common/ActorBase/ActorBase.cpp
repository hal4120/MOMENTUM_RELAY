#include "ActorBase.h"

#include <stdexcept>

#include "../../../Application/Application.h"

#include "../../../Common/Vector2.h"
#include "../../../Common/Vector2I.h"

#include "../../../Manager/TimeScale/TimeScale.h"

#include "../Collider/ColliderBase.h"

#include "../Shader/ShaderBase.h"

#include "../DataLoad/ParameterLoad.h"

ActorBase::ActorBase() :
	trans(),
	colliders(),

	dynamicFlg(true),
	gravityFlg(false),

	pushFlg(true),
	pushWeight(0),

	velocity(),

	isGroundMaster(false),

	isDraw(true),
	drawType(ACTOR_DRAW_TYPE::Normal),

	shader(nullptr),

	parameter(nullptr),

	childActors()
{
}

ActorBase::ActorBase(const std::string& parameterPath) :
	trans(),
	colliders(),

	dynamicFlg(true),
	gravityFlg(false),

	pushFlg(true),
	pushWeight(0),

	velocity(),

	isGroundMaster(false),

	isDraw(true),
	drawType(ACTOR_DRAW_TYPE::Normal),

	shader(nullptr),

	parameter(new ParameterLoad(parameterPath)),

	childActors()
{
}

void ActorBase::Init(void)
{
	// 最終派生固有
	SubInit();

	// 中間基底固有
	BaseInit();

	// 子アクターの初期化
	for (ActorBase* child : childActors) { child->Init(); }

	// モデル制御情報の適用
	trans.Attach();
	
	// パラメータの解放
	if (parameter != nullptr) {
		parameter->Release();
		delete parameter;
		parameter = nullptr;
	}
}

void ActorBase::Update(void)
{
	// 1フレーム前の加速度を保持
	Vector3 prevVelocity = velocity;
	// 動的オブジェクトの場合1フレーム前の座標を保持
	if (dynamicFlg) { trans.prevPos = trans.pos; }

	// 最終派生固有
	SubUpdate();

	// 中間基底固有
	BaseUpdate();

	if (dynamicFlg) {
		// 加速度更新
		VelocityUpdate(velocity == prevVelocity);

		// 接地判定のリセット
		isGroundMaster = false;
	}

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
	if (shader != nullptr) { shader->ResetApply(); }
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

void ActorBase::Gravity(void)
{
	if (!gravityFlg) { return; }

	velocity.y += GRAVITY * TimeScale::Get();
	if (velocity.y < GRAVITY_MAX) { velocity.y = GRAVITY_MAX; }
}

void ActorBase::VelocityUpdate(bool	deceleration)
{
	// 重力処理
	Gravity();

#pragma region 横軸（X,Z）
	// 横軸のみの加速度の大きさを求める
	float widthAccelLen = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);

	// 横軸の加速度がある場合
	if (widthAccelLen > 0.0f) {

		// 補正倍率
		float scale = 1.0f;

		// 減速処理
		if (deceleration) {
			// 横軸の加速度が減速の半分以下の場合は、横軸の加速度を0にする
			if (widthAccelLen <= (DECEL_RATE * TimeScale::Get()) * 0.5f) { velocity.x = velocity.z = 0.0f; }

			// 減速
			scale = (widthAccelLen - (DECEL_RATE * TimeScale::Get())) / widthAccelLen;
		}

		// 最大加速制限～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

		// 最大加速を超えていれば、超過分を打ち消す補正倍率を組み込む
		if (widthAccelLen > ACCEL_MAX) { scale = ACCEL_MAX / widthAccelLen; }

		// 実際に補正
		velocity.x *= scale;
		velocity.z *= scale;
		// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～最大加速制限
	}
#pragma endregion

	// 加速度を座標に反映
	if (velocity != 0.0f) { trans.pos += velocity * TimeScale::Get(); }
}

void ActorBase::AddCollider(ColliderBase* newCollider)
{
	newCollider->SetTransformPtr(&trans);
	newCollider->SetDynamicFlgPtr(&dynamicFlg);
	newCollider->SetPushFlgPtr(&pushFlg);
	newCollider->SetPushWeightPtr(&pushWeight);
	newCollider->SetOnCollisionFunc([this](COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result) { this->OnCollision(ownTag, other, result); });
	newCollider->SetOnGroundedFunc([this](COLLIDER_TAG ownTag, const ColliderBase& other) { this->OnGrounded(ownTag, other); });
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

	// 入力量に応じて加速
	velocity += (vec * ACCEL_RATE) * TimeScale::Get();

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