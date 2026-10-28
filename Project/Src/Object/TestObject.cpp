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

	// コライダーを生成
	AddCollider(new SphereCollider(COLLIDER_TAG::DebugObject, 100.0f));
}

const Vector3& TestObject::VelGreed(void)
{
	// 現在加速度を保持
	Vector3 ret = velocity;

	// 加速度を打ち消し
	velocity = 0.0f;

	// 停止
	SetDynamicFlg(false);

	// 保持した加速度を返す
	return ret;
}

void TestObject::SubUpdate(void)
{
	if (velocityGreed) { return; }

	if (velocity > 0.0f || !isGround) { return; }

	if (intervalCounter > 0) {
		intervalCounter--;
		return;
	}

	intervalCounter = MOVE_INTERVAL;

	velocity = (MOVE_POWER * reverse) + Vector3::Yonly(30.0f);
	reverse *= -1;
}

void TestObject::SubDraw(void)
{
	DrawSphere3D(trans.pos.ToVECTOR(), 100.0f, 16, 0xffffff, 0xffffff, true);
}

void TestObject::SubOnGrounded(COLLIDER_TAG ownTag, const ColliderBase& other, const Vector3& pushVec)
{
	if (!GetDynamicFlg()) { return; }

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