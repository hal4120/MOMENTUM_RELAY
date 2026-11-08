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

	contactResponseEnabled(true),


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

	contactResponseEnabled(true),


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
	// 現在座標を前フレーム座標として保存する
	trans.prevPos = trans.pos;

	// 描画タイプを、このActorのデフォルト設定に戻す
	SetDrawType(DefaultDrawType());

	// 最終派生クラス固有の更新処理を実行する
	SubUpdate();

	// 中間基底クラス固有の更新処理を実行する
	BaseUpdate();

	// PhysicsBodyが登録されているActorのみ物理挙動を更新する。
	if (physicsBody != nullptr) {

		// 取得した移動量を現在座標に加算する
		trans.pos += physicsBody->Step();

		// 「回転軸」の計算結果を受け取るための変数
		Vector3 rotationAxis(0.0f);
		// 「回転角度（ラジアン）」の計算結果を受け取るための変数
		float rotationAngle = 0.0f;

		// PhysicsBodyの角速度から、このフレームの回転量を計算する
		if (physicsBody->StepRotation(rotationAxis, rotationAngle)) {

			// 物理演算によって発生した回転量を追加する
			trans.SetRotation(Quaternion::FromAxisAngle(rotationAxis, rotationAngle) * trans.rotation);
		}
	}

	// このActorが所有している子Actorを順番に更新する
	for (ActorBase* child : childActors) { child->Update(); }

	// シェーダーが登録されている場合、その更新処理を実行する
	if (shader != nullptr) { shader->Update(); }

	// 接地フラグをリセットする
	isGroundMaster = false;
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

void ActorBase::OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)
{
	if (other.GetTag() == COLLIDER_TAG::CameraLine) { SetDrawType(ACTOR_DRAW_TYPE::Alpha); }
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
    
    return taken;
}

void ActorBase::VelGive(const Vector3& vel)
{
	if (physicsBody == nullptr) { return; }

    physicsBody->GiveVelocity(vel);
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

void ActorBase::EnablePhysics(void)
{
	// すでにPhysicsBodyが生成されている場合は何もしない
    if (physicsBody != nullptr) { return; }

	// PhysicsBodyを 生成/保持
    physicsBody = new PhysicsBody();

    // 従来のActorBaseと同様、明示的に有効化するまで重力は無効。
    physicsBody->SetGravityEnabled(false);
}

void ActorBase::SetColliderMobility(COLLIDER_MOBILITY value)
{
	// メンバ変数
	colliderMobility = value;

	// モデル制御情報の動的フラグ
	trans.dynamicFlg = (value == COLLIDER_MOBILITY::Movable);

	// コライダーの動的フラグ
	for (ColliderBase* collider : colliders) {
		if (collider != nullptr) { collider->SetMobility(value); }
	}
	// チャンク区分変更時だけ静的・動的チャンクを再構築
	// （初期化前に呼ばれる可能性があるため、登録済みColliderがある場合のみ）
	// 登録タイミングが不明なため、ここでは再構築を呼ばず、Manager側の明示更新に委ねる。
}

// Actor側からPhysicsBodyを操作するための初期設定API。
// すべて既存のPhysicsBody setterへ委譲し、物理計算は変更しない。
void ActorBase::AddCollider(ColliderBase* newCollider)
{
	newCollider->SetActorPtr(this);
	newCollider->SetMobility(colliderMobility);
	newCollider->SetOnCollisionFunc([this](COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result) { this->OnCollision(ownTag, other, result); });
	newCollider->SetOnPushedFunc([this](COLLIDER_TAG ownTag, const ColliderBase& other, const Vector3& pushVec) { this->OnPushed(ownTag, other, pushVec); });
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

// 位置拘束の角度補正。角速度には加算しない。
void ActorBase::ApplyPhysicsRotation(const Vector3& rotationVector)
{
    const float angle = rotationVector.Length();
    if (angle <= 1.0e-7f) return;
    trans.SetRotation(
        (Quaternion::FromAxisAngle(rotationVector / angle, angle) * trans.rotation).Normalized()
    );
}