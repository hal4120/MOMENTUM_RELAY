#include "CameraCollOperator.h"

#include "../../Manager/Camera/CameraBase.h"

#include "../../Object/Common/Collider/SphereCollider.h"

CameraCollOperator::CameraCollOperator(CameraBase& camera, float radius) : 
	ActorBase(),

	camera(camera)
{
	trans.pos = camera.GetPos();
	trans.SetAngle(camera.GetAngle());

	// コライダー追加
	AddCollider(new SphereCollider(COLLIDER_TAG::Camera, radius));
}

void CameraCollOperator::SubUpdate(void)
{
	trans.pos = camera.GetPos();

	trans.SetAngle(camera.GetAngle());
}