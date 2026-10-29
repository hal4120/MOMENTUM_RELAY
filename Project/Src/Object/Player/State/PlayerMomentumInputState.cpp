#include "PlayerMomentumInputState.h"

#include "../../../Manager/Input/InputManager.h"
#include "../../../Manager/TimeScale/TimeScale.h"

#include "../../../Common/Vector3.h"

PlayerMomentumInputState::PlayerMomentumInputState(
	float STEAL_COOL_TIME,

	float AIM_KEY_HOLD_TIME,

	const std::function<bool(void)>& isSteal,

	const std::function<void(void)>& changeStateQuick,
	const std::function<void(void)>& changeStateAim
):
	STEAL_COOL_TIME(STEAL_COOL_TIME),

	AIM_KEY_HOLD_TIME(AIM_KEY_HOLD_TIME),

	isSteal(isSteal),

	changeStateQuick(changeStateQuick),
	changeStateAim(changeStateAim),

	keyHoldCounter(),
	stealCoolTimeCounter()
{
}

void PlayerMomentumInputState::OwnStateConditionUpdate(void)
{
	// 勢いを保持している -> 勢いを「与える」行動
	if (isSteal()) {

	}
	// 勢いを保持していない -> 勢いを「奪う」行動
	else {
		// クールタイム中
		if (stealCoolTimeCounter > 0.0f) { return; }
	}

	// ダウントリガーで遷移
	if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMomentumSteal).down) { OwnChangeState(); }
}

void PlayerMomentumInputState::Enter(void)
{
	// 長押し時間計測カウンターをリセット
	keyHoldCounter = 0.0f;
}

void PlayerMomentumInputState::Update(void)
{
	// 長押し時間に達するまえにアップトリガーの検出で「クイック発動」
	if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMomentumSteal).up) {

		// クイック発動
		changeStateQuick();

		return;
	}

	// 長押し時間更新
	keyHoldCounter += TimeScale::Get();

	// 長押し時間が達していたら、「エイム発動」
	if (keyHoldCounter >= AIM_KEY_HOLD_TIME) {

		// エイム発動
		changeStateAim();

		return;
	}
}

void PlayerMomentumInputState::Exit(void)
{
	// 長押し時間計測カウンターをリセット
	keyHoldCounter = 0.0f;

	// 勢いを「奪う」行動のクールタイムを設定
	if (!isSteal()) {
		stealCoolTimeCounter = STEAL_COOL_TIME;
	}
}

void PlayerMomentumInputState::AlwaysUpdate(void)
{
	// 自分の状態じゃないとき
	if (!IsOwnState()) {

		// クールタイム中
		if (stealCoolTimeCounter > 0.0f) {

			// クールタイムを減らす
			stealCoolTimeCounter -= TimeScale::Get();

			// 0未満にならないように安全処理
			if (stealCoolTimeCounter < 0.0f) { stealCoolTimeCounter = 0.0f; }
		}
	}
}
