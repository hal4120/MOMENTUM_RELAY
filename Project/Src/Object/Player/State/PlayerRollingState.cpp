#include "PlayerRollingState.h"

#include "../../../Common/Vector3.h"
#include "../../../Common/Quaternion.h"

#include "../../../Manager/Input/InputManager.h"

#include "../../../Manager/TimeScale/TimeScale.h"

PlayerRollingState::PlayerRollingState(
	float ACCEL_MAX,

	int COOL_TIME,

	float INVI_START_TIME,
	float INVI_END_TIME,

	const Quaternion& rotation,

	std::function<void(const Vector3&)> moveAccel,
	float& accelMax,

	bool& isInvincible,

	std::function<void(void)> playDodgeAnime,
	std::function<float(void)> getAnimePlayRatio,
	std::function<void(void)> defaultChangeState
):
	ACCEL_MAX(ACCEL_MAX),

	COOL_TIME(COOL_TIME),

	INVI_START_TIME(INVI_START_TIME),
	INVI_END_TIME(INVI_END_TIME),

	rotation(rotation),

	moveAccel(moveAccel),
	accelMax(accelMax),

	isInvincible(isInvincible),

	playDodgeAnime(playDodgeAnime),
	getAnimePlayRatio(getAnimePlayRatio),
	defaultChangeState(defaultChangeState),

	step(STEP::Startup),
	coolTimeCounter()
{
}

void PlayerRollingState::OwnStateConditionUpdate(void)
{
	// 攻撃のクールタイム中は状態遷移しない
	if (coolTimeCounter > 0) { return; }

	// 攻撃キーのダウントリガーで状態遷移
	if (Input::GetIns().GetInfo(KEY_TYPE::PlayerRolling).down) { OwnChangeState(); }
}

void PlayerRollingState::Enter(void)
{
	// ステップを「前隙」へ
	step = STEP::Startup;

	// 移動速度をデフォルトに
	accelMax = ACCEL_MAX;

	// 無敵判定無効
	isInvincible = false;

	// アニメーションの再生
	playDodgeAnime();

	// 攻撃のクールタイムをセットする
	coolTimeCounter = (float)COOL_TIME;
}

void PlayerRollingState::Update(void)
{
	// アニメーションの再生割合を取得
	const float animeRatio = getAnimePlayRatio();

	// ステップ別更新
	switch (step) {

	case STEP::Startup: {
		// 前隙

		// 攻撃判定発生開始
		if (INVI_START_TIME <= animeRatio) {

			// ステップを「無敵判定発生中」へ
			step = STEP::Active;

			// 無敵判定有効
			isInvincible = true;

			// 角度の方向に移動する
			float movePower = 75.0f;
			moveAccel(rotation.Rotate(Vector3::Zonly(movePower)));
			accelMax = movePower;
		}

		break;
	}

	case STEP::Active: {
		// 攻撃判定発生中

		// 攻撃判定発生終了
		if (animeRatio <= INVI_END_TIME) {

			// ステップを「後隙」へ
			step = STEP::Recovery;

			// 無敵判定無効
			isInvincible = false;
		}

		break;
	}

	case STEP::Recovery: {
		// 後隙

		// アニメーション再生終了で強制的に待機状態へ
		if (1.0f <= animeRatio) {

			// 待機状態へ遷移
			defaultChangeState();
		}

		break;
	}

	}
}

void PlayerRollingState::Exit(void)
{
	// 無敵判定無効
	isInvincible = false;

	// 移動速度をデフォルトに
	accelMax = ACCEL_MAX;
}

void PlayerRollingState::AlwaysUpdate(void)
{
	// 自身の状態でないときは、攻撃のクールタイムを減らす
	if (!IsOwnState()) {
		// 攻撃のクールタイムを減らす
		if (coolTimeCounter > 0.0f) {
			coolTimeCounter -= TimeScale::Get();

			if (coolTimeCounter <= 0.0f) { coolTimeCounter = 0.0f; }
		}
	}
}
