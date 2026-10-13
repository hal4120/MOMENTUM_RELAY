#include "ColliderBase.h"

#include <cassert>

#include "../ActorBase/ActorBase.h"

CollisionResult::CollisionResult(void) :
	point(),
	normal(),
	penetration(0.0f)
{
}


ColliderBase::ColliderBase(COLLIDER_TAG type, const Vector3& pos, const Quaternion& rotation) :
	owner(nullptr),

	pos(pos),
	rotation(rotation.Normalized()),

	judgeFlg(true),

	tag(type),
	shape(COLLIDER_SHAPE::None),

	OnCollision(nullptr),
	OnGrounded(nullptr)
{
}


void ColliderBase::SetActorPtr(ActorBase* ptr)
{
	owner = ptr;
}

void ColliderBase::SetOnCollisionFunc(std::function<void(COLLIDER_TAG, const ColliderBase&, const CollisionResult&)> OnCollisionFunc)
{
	OnCollision = std::move(OnCollisionFunc);
}


void ColliderBase::SetOnGroundedFunc(std::function<void(COLLIDER_TAG, const ColliderBase&)> OnGroundedFunc)
{
	OnGrounded = std::move(OnGroundedFunc);
}


Vector3 ColliderBase::GetPos(void) const
{
	assert(owner != nullptr);

	/*
		相対座標posはTransform本体の回転のみを受ける。

		Collider自身のrotationは、
		Colliderの形状を回転させるためのものなので
		相対座標には適用しない。
	*/
	return owner->GetTrans().pos + owner->GetTrans().VTrans(pos);
}


Vector3 ColliderBase::GetPrevPos(void) const
{
	assert(owner != nullptr);

	return owner->GetTrans().prevPos + owner->GetTrans().VTrans(pos);
}


Quaternion ColliderBase::GetRotation(void) const
{
	assert(owner != nullptr);

	/*
		Transform本体の回転
			↓
		Collider固有の相対回転

		の順番で合成する。

		現段階では既存ActionBaseの行列規約を
		確実に維持するため、DxLibの行列で合成してから
		Quaternionへ変換する。
	*/
	return Quaternion::FromMatrix(GetRotationMat());
}


MATRIX ColliderBase::GetRotationMat(void) const
{
	assert(owner != nullptr);

	return MMult(owner->GetTrans().RotationMat(), rotation.ToMatrix());
}


Vector3 ColliderBase::VTrans(const Vector3& v) const
{
	if (v == 0.0f) { return Vector3(); }

	return Vector3(VTransform(v.ToVECTOR(), GetRotationMat()));
}


const Transform& ColliderBase::GetTransform(void) const
{
	assert(owner != nullptr);

	return owner->GetTrans();
}


bool ColliderBase::GetDynamicFlg(void) const
{
	assert(owner != nullptr);

	return owner->GetDynamicFlg();
}


bool ColliderBase::GetJudgeFlg(void) const
{
	return judgeFlg;
}


bool ColliderBase::GetPushFlg(void) const
{
	assert(owner != nullptr);

	return owner->GetPushFlg();
}


unsigned char ColliderBase::GetPushWeight(void) const
{
	assert(owner != nullptr);

	return owner->GetPushWeight();
}


COLLIDER_TAG ColliderBase::GetTag(void) const
{
	return tag;
}


COLLIDER_SHAPE ColliderBase::GetShape(void) const
{
	return shape;
}


void ColliderBase::CallOnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)
{
	if (!OnCollision) { return; }

	OnCollision(ownTag, other, result);
}


void ColliderBase::CallOnGrounded(COLLIDER_TAG ownTag, const ColliderBase& other)
{
	if (!OnGrounded) { return; }

	OnGrounded(ownTag, other);
}


void ColliderBase::SetTransformPos(const Vector3& pos)
{
	assert(owner != nullptr);

	owner->SetPos(pos);
}


void ColliderBase::SetTransformPosAdd(const Vector3& vec)
{
	assert(owner != nullptr);

	owner->SetPos(owner->GetTrans().pos + vec);
}


void ColliderBase::SetJudgeFlg(bool flg)
{
	judgeFlg = flg;
}


void ColliderBase::SetShape(COLLIDER_SHAPE s)
{
	shape = s;
}