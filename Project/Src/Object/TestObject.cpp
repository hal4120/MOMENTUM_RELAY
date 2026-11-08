#include "Common/PhysicsBody/PhysicsBody.h"
#include "TestObject.h"

#include "../Utility/Utility.h"

#include "Common/Collider/SphereCollider.h"

int TestObject::modelHandleOriginal = -1;

TestObject::TestObject(const Vector3& initPos, const Vector3& MOVE_POWER) :
	ActorBase(),

	MOVE_POWER(MOVE_POWER),

	velocityGreed(false),

	intervalCounter(0),
	reverse(1)
{
	trans.pos = initPos;
}

void TestObject::Load(void)
{
#pragma region アクター挙動設定

	// 動的オブジェクト
	SetColliderMobility(COLLIDER_MOBILITY::Movable);

	// 物理演算有効化
	EnablePhysics();

	// 物理演算管理取得
	PhysicsBody* body = GetPhysicsBody();

	// 重力有効
	body->SetGravityEnabled(true);
	// 押し出し有効
	SetContactResponseEnabled(true);
	// 物理衝突時の重さを設定
	body->SetMass(0.5f);

	// 回転有効（回転を無効にすると、摩擦が効かなくなる）
	body->SetRotationEnabled(true);
	// 静止摩擦（0.8f程度が自然な挙動になる）
	body->SetStaticFriction(0.8f);
	// 動摩擦（0.6f程度が自然な挙動になる）
	body->SetDynamicFriction(0.6f);
	// 回転減衰（0.005f程度が自然な挙動になる）
	body->SetAngularDragPerFrame(0.005f);
	// 水平移動の減衰（0.05f程度が自然な挙動になる）
	body->SetHorizontalDragPerFrame(0.05f);
	// 転がり抵抗（0.001f程度が自然な挙動になる）
	body->SetRollingResistance(0.001f);
	// 落下速度制限（落下速度が速すぎると衝突判定が抜けることがあるため）
	body->SetMaxFallSpeed(100.0f);

#pragma endregion

	// 静的に定義したハンドル番号格納変数を使いまわす
	if (modelHandleOriginal == -1) { modelHandleOriginal = MV1LoadModel("Data/Model/Moon/Moon.mv1"); }

	// モデルのハンドルを設定
	trans.model = modelHandleOriginal;

	// モデルのスケールを設定（モデルの大きさが大きすぎるため）
	trans.scale = 0.57282954884f;

	// 球体の半径を算出
	const float radius = 174.572f * trans.scale.MaxElementF();

	// コライダーを球体に設定（半径はモデルの大きさに合わせる）
	AddCollider(new SphereCollider(COLLIDER_TAG::DebugObject, radius));

	// 慣性モーメントを球体に設定（半径はコライダーと同じ）
	body->SetShapeInertiaSphere(radius);
}

void TestObject::SubUpdate(void)
{
	if (!(GetColliderMobility() == COLLIDER_MOBILITY::Movable)) { return; }

	if ((GetPhysicsBody() != nullptr && GetPhysicsBody()->GetVelocity() > 0.0f) || !isGround) { return; }

	if (intervalCounter > 0) {
		intervalCounter--;
		return;
	}

	intervalCounter = MOVE_INTERVAL;

	if (GetPhysicsBody() != nullptr) { GetPhysicsBody()->SetVelocity((MOVE_POWER * reverse) + Vector3::Yonly(30.0f)); }
	reverse *= -1;
}

void TestObject::SubRelease(void)
{
	if (modelHandleOriginal != -1) {
		MV1DeleteModel(modelHandleOriginal);
		modelHandleOriginal = -1;
	}
}

void TestObject::SubOnPushed(COLLIDER_TAG ownTag, const ColliderBase& other, const Vector3& pushVec)
{
    // 反発・摩擦はCollisionManagerとPhysicsBodyが処理する。
}
