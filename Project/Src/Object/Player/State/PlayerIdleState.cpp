#include "PlayerIdleState.h"

#include "../../../Common/Vector3.h"

#include "../../../Manager/Input/InputManager.h"

PlayerIdleState::PlayerIdleState(std::function<void(void)> playAnimeIdle):
	playAnimeIdle(playAnimeIdle)
{
}

void PlayerIdleState::OwnStateConditionUpdate(void)
{
	Vector3 vec = Vector3();

	// コントローラーの入力を取得
	vec = Input::GetIns().GetLeftStickVec(false).ToVector3XZInvertY();

	// 入力がなければ次にキーボードの入力を取得
	if (vec == 0.0f) {
		if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMoveRight).now) { vec.x++; }
		if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMoveLeft).now) { vec.x--; }
		if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMoveFront).now) { vec.z++; }
		if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMoveBack).now) { vec.z--; }
	}

	// 移動入力がなければ、自分の状態に遷移
	if (vec == 0.0f) { OwnChangeState(); }
}

void PlayerIdleState::Enter(void)
{
	playAnimeIdle();
}