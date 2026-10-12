#include "PlayerAttackState.h"

#include "../../Common/Transform/Transform.h"

#include "../../../Manager/Input/InputManager.h"

#include "../Wepon/PlayerAttackCollOperator.h"

PlayerAttackState::PlayerAttackState(
	float COLL_START_TIME,
	float COLL_END_TIME,

	PlayerAttackCollOperator& collOperator,

	std::function<void(void)> playAnimeAttack,
	std::function<float(void)> getAnimeRatio,

	std::function<void(void)> changeStateIdle
) :
	COLL_START_TIME(COLL_START_TIME),
	COLL_END_TIME(COLL_END_TIME),

	collOperator(collOperator),

	playAnimeAttack(playAnimeAttack),
	getAnimeRatio(getAnimeRatio),

	changeStateIdle(changeStateIdle),

	step()
{
}

void PlayerAttackState::OwnStateConditionUpdate(void)
{
	if (Input::GetIns().GetInfo(KEY_TYPE::PlayerKickDownAttack).down) {
		OwnChangeState();
	}
}

void PlayerAttackState::Enter(void)
{
	// ステップを「前隙」へ
	step = STEP::Startup;

	// 当たり判定を消去
	collOperator.Off();

	// 攻撃アニメーション再生
	playAnimeAttack();
}

void PlayerAttackState::Update(void)
{
	// アニメーションの再生割合を取得
	const float animeRatio = getAnimeRatio();

	// ステップ別更新
	switch (step) {

	case PlayerAttackState::STEP::Startup: {
		// 前隙

		// 攻撃判定発生開始
		if (COLL_START_TIME <= animeRatio) {

			// ステップを「攻撃判定発生中」へ
			step = STEP::Active;

			// 当たり判定を発生
			collOperator.On();
		}

		break;
	}

	case PlayerAttackState::STEP::Active: {
		// 攻撃判定発生中

		// 当たり判定を追従
		
		// 攻撃判定発生終了
		if (animeRatio <= COLL_END_TIME) {

			// ステップを「後隙」へ
			step = STEP::Recovery;

			// 当たり判定を消去
			collOperator.Off();
		}

		break;
	}

	case PlayerAttackState::STEP::Recovery: {
		// 後隙

		// アニメーション再生終了で強制的に待機状態へ
		if (1.0f <= animeRatio) {

			// 待機状態へ遷移
			changeStateIdle();
		}

		break;
	}

	}
}

void PlayerAttackState::Exit(void)
{
	// 当たり判定を消去
	collOperator.Off();
}