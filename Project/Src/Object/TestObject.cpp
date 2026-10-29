#include "TestObject.h"

#include "../Utility/Utility.h"

#include "Common/Collider/SphereCollider.h"

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
	SetDynamicFlg(true);

	// 重力有効
	SetGravityFlg(true);

	// 押し出し有効
	SetPushFlg(true);

	// 押し出し重み設定
	SetPushWeight(50);

#pragma endregion

	trans.LoadModel("Moon/Moon");

	trans.scale = 0.57282954884f;

	// コライダーを生成
	AddCollider(new SphereCollider(COLLIDER_TAG::DebugObject, 174.572f * trans.scale.MaxElementF()));
}

void TestObject::SubUpdate(void)
{
	if (!GetDynamicFlg()) { return; }

	if (velocity > 0.0f || !isGround) { return; }

	if (intervalCounter > 0) {
		intervalCounter--;
		return;
	}

	intervalCounter = MOVE_INTERVAL;

	velocity = (MOVE_POWER * reverse) + Vector3::Yonly(30.0f);
	reverse *= -1;
}

void TestObject::SubOnPushed(COLLIDER_TAG ownTag, const ColliderBase& other, const Vector3& pushVec)
{
	if (!GetDynamicFlg()) { return; }

	// 反射挙動
	ReflexiveBehavior(velocity, pushVec);
}