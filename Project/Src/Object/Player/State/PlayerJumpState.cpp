#include "PlayerJumpState.h"

#include "../../../Common/Vector3.h"

#include "../../../Manager/Input/InputManager.h"
#include "../../../Manager/Camera/CurrentCamera.h"

PlayerJumpState::PlayerJumpState(
	float JUMP_POWER,

	float& velocityY,
	const bool& isGround,

	std::function<void(const Vector3&)> moveAccel,

	std::function<void(void)> playAnimeJumpStart,
	std::function<void(void)> playAnimeJumpLoop,
	std::function<void(void)> playAnimeStamp,

	std::function<bool(void)> isAnimeEnd,

	std::function<void(void)> changeStateIdle
):
	JUMP_POWER(JUMP_POWER),

	velocityY(velocityY),
	isGround(isGround),

	moveAccel(moveAccel),

	playAnimeJumpStart(playAnimeJumpStart),
	playAnimeJumpLoop(playAnimeJumpLoop),
	playAnimeStamp(playAnimeStamp),

	isAnimeEnd(isAnimeEnd),

	changeStateIdle(changeStateIdle),

	step(STEP::Start)
{
}

void PlayerJumpState::OwnStateConditionUpdate(void)
{
	// ジャンプキーのダウントリガーで遷移
	if (Input::GetIns().GetInfo(KEY_TYPE::PlayerJump).down) { OwnChangeState(); }
}

void PlayerJumpState::Enter(void)
{
	// フラグリセット
	step = STEP::Start;

	// ジャンプ開始アニメを再生
	playAnimeJumpStart();
}

void PlayerJumpState::Update(void)
{
	switch (step) {

	case PlayerJumpState::STEP::Start: {
		// ジャンプ開始

		// アニメーション終了
		if (isAnimeEnd()) {

			// 加速度加算
			velocityY = JUMP_POWER;

			// 滞空アニメーションを再生
			playAnimeJumpLoop();

			// 次のステップへ
			step = STEP::Loop;
		}

		break;
	}

	case PlayerJumpState::STEP::Loop: {
		// 滞空

		// コントローラーの入力を取得
		Vector3 vec = Input::GetIns().GetLeftStickVec(false).ToVector3XZInvertY();

		// 入力がなければ次にキーボードの入力を取得
		if (vec == 0.0f) {
			if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMoveRight).now) { vec.x++; }
			if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMoveLeft).now) { vec.x--; }
			if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMoveFront).now) { vec.z++; }
			if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMoveBack).now) { vec.z--; }
			vec.Normalize();
		}

		// 最終的に入力があれば移動させる
		if (vec != 0.0f) {

			// 移動方向をカメラで回転させる
			vec.TransMatOwn(MGetRotY(CurrentCamera::Get().GetAngle().y));

			// 移動
			moveAccel(vec);
		}

		// 着地
		if (isGround) {

			// 着地のアニメーションを再生
			playAnimeStamp();

			// 次のステップへ
			step = STEP::Stamp;
		}

		break;
	}

	case PlayerJumpState::STEP::Stamp: {
		// 着地

		// この時点から入力によって再度ジャンプが可能にしておく
		OwnStateConditionUpdate();

		// アニメーション終了
		if (isAnimeEnd()) {

			// 待機状態に戻り終了
			changeStateIdle();
		}

		break;
	}

	}
}