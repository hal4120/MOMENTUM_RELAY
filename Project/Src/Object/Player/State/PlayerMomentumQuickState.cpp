#include "PlayerMomentumQuickState.h"

#include "../../../Manager/TimeScale/TimeScale.h"

#include "../Weapon/PlayerMomentumQuickStealFind.h"

PlayerMomentumQuickState::PlayerMomentumQuickState(
	float STEAL_FIND_LIMIT_TIME,

	const std::function<bool(void)>& isSteal,

	ActorBase* player,
	ActorBase*& target,

	PlayerMomentumQuickStealFind& collOperator,

	const std::function<void(void)>& changeStateSteal,
	const std::function<void(void)>& changeStateDefault
) :
	STEAL_FIND_LIMIT_TIME(STEAL_FIND_LIMIT_TIME),

	isSteal(isSteal),

	player(player),
	target(target),

	collOperator(collOperator),

	changeStateMomentum(changeStateSteal),
	changeStateDefault(changeStateDefault),

	stealFindLimitTimeCounter()
{
}

void PlayerMomentumQuickState::Enter(void)
{
	// 勢いを保持している -> 勢いを「与える」行動
	if (isSteal()) {
		//「与える」を「クイック発動」の場合は自分自身に対する付与とする

		// プレイヤー自身をターゲットにする
		target = player;

		// 即時、実際に勢い操作を実行する状態へ
		changeStateMomentum();
	}
	// 勢いを保持していない -> 勢いを「奪う」行動
	else {
		// 探索開始（当たり判定）
		collOperator.Find();

		// 探索猶予時間計測カウンターを初期化
		stealFindLimitTimeCounter = 0.0f;
	}
}

void PlayerMomentumQuickState::Update(void)
{
	// 探索結果を格納
	target = collOperator.GetTarget();

	// 探索成功で、実際に勢い操作を実行する状態に遷移
	if (target) {

		// 遷移
		changeStateMomentum();

		// 終了
		return;
	}

	// 探索猶予時間計測カウンター更新
	stealFindLimitTimeCounter += TimeScale::Get();

	// 探索成功を前に制限時間に達したら
	if (stealFindLimitTimeCounter >= STEAL_FIND_LIMIT_TIME) {

		// 不発として通常状態に戻す
		changeStateDefault();

		// 終了
		return;
	}
}