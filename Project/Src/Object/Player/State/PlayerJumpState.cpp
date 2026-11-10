#include "PlayerJumpState.h"

#include "../../../Common/Vector3.h"

#include "../../../Manager/Input/InputManager.h"
#include "../../../Manager/Camera/CurrentCamera.h"

#include "../../Common/MotionController/GroundMotionController.h"

#include "../Player.h"

PlayerJumpState::PlayerJumpState(
	GroundMotionController& motion,

	const std::function<void(void)>& playAnimeJumpStart,
	const std::function<void(void)>& playAnimeJumpLoop,
	const std::function<void(void)>& playAnimeStamp,

	const std::function<bool(void)>& isAnimeEnd,

	const std::function<void(void)>& changeStateIdle
):
	motion(motion),

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
	// 最初のカメライベント中は状態遷移しない
	if (Player::isFirstCameraEvent) { return; }

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

			// ジャンプ開始
			motion.TryJump();

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
			vec.TransMatOwn(MGetRotY(CurrentCamera::Get().GetRotation().GetYaw()));

			// 移動
			motion.ApplyInput(vec);
		}

		// 着地
		if (motion.IsGrounded()) {

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