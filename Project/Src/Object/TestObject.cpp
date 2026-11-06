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

	EnablePhysics();

	PhysicsBody* body = GetPhysicsBody();

	body->SetRotationEnabled(true);
	body->SetStaticFriction(0.8f);
	body->SetDynamicFriction(0.6f);
	body->SetAngularDragPerFrame(0.005f);
	body->SetHorizontalDragPerFrame(0.05f);
	body->SetRollingResistance(0.001f);
	body->SetMaxFallSpeed(100.0f);

	// 重力有効
	body->SetGravityEnabled(true);

	// 押し出し有効
	SetContactResponseEnabled(true);

	// 押し出し重み設定
#pragma endregion

	if (modelHandleOriginal == -1) { modelHandleOriginal = MV1LoadModel("Data/Model/Moon/Moon.mv1"); }

	//trans.LoadModel("Moon/Moon");
	trans.model = modelHandleOriginal;


	trans.scale = 0.57282954884f;

	// コライダーと慣性モーメントで同じ半径を使う
	const float radius = 174.572f * trans.scale.MaxElementF();
	AddCollider(new SphereCollider(COLLIDER_TAG::DebugObject, radius));
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
