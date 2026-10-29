#pragma once

#include "../../Common/CharacterBase/CharacterStateBase.h"

class ActorBase;

struct Transform;

class PlayerMomentumQuickStealFind;

class PlayerMomentumQuickState : public CharacterStateBase
{
public:

	/// <summary>
	/// 勢い操作「クイック発動」（自動ターゲット管理）
	/// </summary>
	/// <param name="STEAL_FIND_LIMIT_TIME">「奪う」のときターゲット探索の猶予時間（フレーム数）</param>
	/// <param name="isSteal">現在勢いを奪って保持しているか</param>
	/// <param name="player">プレイヤーのポインタ</param>
	/// <param name="target">ターゲット格納先</param>
	/// <param name="collOperator">当たり判定管理</param>
	/// <param name="changeStateSteal">実際に勢い操作を実行する状態へ遷移する関数</param>
	/// <param name="changeStateDefault">通常状態へ遷移する関数</param>
	PlayerMomentumQuickState(
		float STEAL_FIND_LIMIT_TIME,

		const std::function<bool(void)>& isSteal,

		ActorBase* player,
		ActorBase*& target,

		PlayerMomentumQuickStealFind& collOperator,

		const std::function<void(void)>& changeStateSteal,
		const std::function<void(void)>& changeStateDefault
	);

	~PlayerMomentumQuickState()override = default;

	// 状態遷移後1度行う初期化処理
	void Enter(void)override;
	// 更新処理
	void Update(void)override;

private:

	// 探索猶予時間（フレーム数）
	const float STEAL_FIND_LIMIT_TIME;

#pragma region 参照

	// 現在勢いを奪って保持しているか
	const std::function<bool(void)> isSteal;

	// プレイヤーのポインタ
	ActorBase* player;

	// ターゲット格納先
	ActorBase*& target;

	// 当たり判定管理
	PlayerMomentumQuickStealFind& collOperator;

	// 実際に勢い操作を実行する状態へ遷移する関数
	const std::function<void(void)> changeStateMomentum;
	// 通常状態へ遷移する関数
	const std::function<void(void)> changeStateDefault;

#pragma endregion

	// 探索猶予時間（フレーム数）計測
	float stealFindLimitTimeCounter;
};