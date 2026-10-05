#include "ColliderBase.h"

#include <cassert>


CollisionResult::CollisionResult(void) :
	point(),
	normal(),
	penetration(0.0f)
{
}


ColliderBase::ColliderBase(COLLIDER_TAG type, const Vector3& pos, const Quaternion& rotation) :
	trans(nullptr),

	dynamicFlg(nullptr),
	pushFlg(nullptr),
	pushWeight(nullptr),

	pos(pos),
	rotation(rotation.Normalized()),

	judgeFlg(true),

	tag(type),
	shape(COLLIDER_SHAPE::None),

	OnCollision(nullptr),
	OnGrounded(nullptr)
{
}


void ColliderBase::SetTransformPtr(Transform* ptr)
{
	trans = ptr;
}


void ColliderBase::SetDynamicFlgPtr(const bool* ptr)
{
	dynamicFlg = ptr;
}


void ColliderBase::SetPushFlgPtr(const bool* ptr)
{
	pushFlg = ptr;
}


void ColliderBase::SetPushWeightPtr(const unsigned char* ptr)
{
	pushWeight = ptr;
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
	assert(trans != nullptr);

	/*
		相対座標posはTransform本体の回転のみを受ける。

		Collider自身のrotationは、
		Colliderの形状を回転させるためのものなので
		相対座標には適用しない。
	*/
	return		trans->pos + trans->VTrans(pos);
}


Vector3 ColliderBase::GetPrevPos(void) const
{
	assert(trans != nullptr);

	return		trans->prevPos + trans->VTrans(pos);
}


Quaternion ColliderBase::GetRotation(void) const
{
	assert(trans != nullptr);

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
	assert(trans != nullptr);

	return MMult(trans->RotationMat(), rotation.ToMatrix());
}


Vector3 ColliderBase::VTrans(const Vector3& v) const
{
	if (v == 0.0f) { return Vector3(); }

	return Vector3(VTransform(v.ToVECTOR(), GetRotationMat()));
}


const Transform& ColliderBase::GetTransform(void) const
{
	assert(trans != nullptr);

	return *trans;
}


bool ColliderBase::GetDynamicFlg(void) const
{
	return (dynamicFlg != nullptr) ? *dynamicFlg : true;
}


bool ColliderBase::GetJudgeFlg(void) const
{
	return judgeFlg;
}


bool ColliderBase::GetPushFlg(void) const
{
	return (pushFlg != nullptr) ? *pushFlg : true;
}


unsigned char ColliderBase::GetPushWeight(void) const
{
	return (pushWeight != nullptr) ? *pushWeight : 0;
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
	assert(trans != nullptr);

	trans->pos = pos;
}


void ColliderBase::SetTransformPosAdd(const Vector3& vec)
{
	assert(trans != nullptr);

	trans->pos += vec;
}


void ColliderBase::SetJudgeFlg(bool flg)
{
	judgeFlg = flg;
}


void ColliderBase::SetShape(COLLIDER_SHAPE s)
{
	shape = s;
}