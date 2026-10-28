#include "PlayerMomentumAimState.h"

#include "../../../Manager/Collision/CurrentCollisionManager.h"
#include "../../../Manager/Camera/CurrentCamera.h"

PlayerMomentumAimState::PlayerMomentumAimState(
	float LIMIT_TIME,

	float TIME_SCALE,

	float RAYCAST_LENGTH,

	ActorBase*& target
) :
	LIMIT_TIME(LIMIT_TIME),

	TIME_SCALE(TIME_SCALE),

	RAYCAST_LENGTH(RAYCAST_LENGTH),

	target(target),

	limitTimeCounter()
{
}

void PlayerMomentumAimState::Enter(void)
{
}

void PlayerMomentumAimState::Update(void)
{
	RaycastHit result =
		CurrentCollisionManager::Get().Raycast(
			CurrentCamera::Get().GetPos(),
			CurrentCamera::Get().GetRotation().Rotate(Vector3::Zonly(1.0f)),
			RAYCAST_LENGTH
		);

	// 当たっていたら
	if (result.IsHit()) {




	}
	// 当たっていなかったら
	else {

	}
}

void PlayerMomentumAimState::Exit(void)
{
}

void PlayerMomentumAimState::ChangeTarget(ActorBase* target)
{
	// 同じだったら何もしない
	if (this->target == target) { return; }

	// 選択状態を解除
	//this->target->

	// 違ったら新しいターゲットに
	this->target = target;

	// 過去の
}
