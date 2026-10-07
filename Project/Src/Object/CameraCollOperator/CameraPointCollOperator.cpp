#include "CameraPointCollOperator.h"

#include "../../Manager/Camera/CameraBase.h"

#include "../Common/Collider/SphereCollider.h"

CameraPointCollOperator::CameraPointCollOperator(const CameraBase& camera, float radius) : 
	ActorBase(),

	cameraPos(camera.GetPos())
{
	trans.pos = cameraPos;

	// コライダー追加
	AddCollider(new SphereCollider(COLLIDER_TAG::CameraPoint, radius));
}

CameraPointCollOperator::CameraPointCollOperator(const Vector3& cameraPos, float radius) :
	ActorBase(),

	cameraPos(cameraPos)
{
	trans.pos = cameraPos;

	// コライダー追加
	AddCollider(new SphereCollider(COLLIDER_TAG::CameraPoint, radius));
}

void CameraPointCollOperator::SubUpdate(void)
{
	trans.pos = cameraPos;
}