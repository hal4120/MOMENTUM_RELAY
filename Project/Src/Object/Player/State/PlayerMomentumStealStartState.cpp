#include "PlayerMomentumStealStartState.h"

#include "../../../Manager/Input/InputManager.h"
#include "../../../Manager/TimeScale/TimeScale.h"

PlayerMomentumStealStartState::PlayerMomentumStealStartState(
	float COOL_TIME,

	float AIM_KEY_HOLD_TIME,

	const std::function<void(void)>& changeStateQuick,
	const std::function<void(void)>& changeStateAim
):
	COOL_TIME(COOL_TIME),

	AIM_KEY_HOLD_TIME(AIM_KEY_HOLD_TIME),

	changeStateQuick(changeStateQuick),
	changeStateAim(changeStateAim),

	keyHoldCounter(),
	coolTimeCounter()
{
}

void PlayerMomentumStealStartState::OwnStateConditionUpdate(void)
{
	// クールタイム中
	if (coolTimeCounter > 0.0f) { return; }

	// ダウントリガーで遷移
	if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMomentumSteal).down) { OwnChangeState(); }
}

void PlayerMomentumStealStartState::Enter(void)
{
	// 長押し時間計測カウンターをリセット
	keyHoldCounter = 0.0f;
}

void PlayerMomentumStealStartState::Update(void)
{
	// 長押し時間に達するまえにアップトリガーの検出で「クイック発動」
	if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMomentumSteal).up) {

		// クイック発動
		changeStateQuick();

		return;
	}

	// 長押し時間が達していたら、「エイム発動」
	if (keyHoldCounter >= AIM_KEY_HOLD_TIME) {

		// エイム発動
		changeStateAim();

		return;
	}

	// 長押し時間更新
	keyHoldCounter += TimeScale::Get();
}

void PlayerMomentumStealStartState::Exit(void)
{
	// 長押し時間計測カウンターをリセット
	keyHoldCounter = 0.0f;

	// クールタイムを設定
	coolTimeCounter = COOL_TIME;
}

void PlayerMomentumStealStartState::AlwaysUpdate(void)
{
	// 自分の状態じゃないとき
	if (!IsOwnState()) {

		// クールタイム中
		if (coolTimeCounter > 0.0f) {

			// クールタイムを減らす
			coolTimeCounter -= TimeScale::Get();

			// 0未満にならないように安全処理
			if (coolTimeCounter < 0.0f) { coolTimeCounter = 0.0f; }
		}
	}
}
