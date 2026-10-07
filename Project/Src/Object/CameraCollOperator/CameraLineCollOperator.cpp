#include "CameraLineCollOperator.h"

#include "../../Manager/Camera/CameraBase.h"

#include "../Common/Collider/CapsuleCollider.h"

CameraLineCollOperator::CameraLineCollOperator(const CameraBase& camera, const Vector3& cameraFollowPos, float radius):
	ActorBase(),

	cameraPos(camera.GetPos()),
	cameraFollowPos(cameraFollowPos)
{
	trans.pos = cameraPos;

	// コライダー生成（操作可能なように保持しておく）
	capsuleCollider = new CapsuleCollider(COLLIDER_TAG::CameraLine, cameraFollowPos, cameraPos, radius);
	// コライダー追加
	AddCollider(capsuleCollider);
}

CameraLineCollOperator::CameraLineCollOperator(const Vector3& cameraPos, const Vector3& cameraFollowPos, float radius):
	ActorBase(),
	cameraPos(cameraPos),
	cameraFollowPos(cameraFollowPos)
{
	trans.pos = cameraPos;

	// コライダー生成（操作可能なように保持しておく）
	capsuleCollider = new CapsuleCollider(COLLIDER_TAG::CameraLine, Vector3(), cameraFollowPos - cameraPos, radius);
	// コライダー追加
	AddCollider(capsuleCollider);
}

void CameraLineCollOperator::SubUpdate(void)
{
	// 座標をカメラ座標に合わせる
	trans.pos = cameraPos;

	// カプセルが伸びる相対座標を設定する
	capsuleCollider->SetEndPos((cameraFollowPos - cameraPos) * 0.8f);
}
