#pragma once

#include "../../Common/CharacterBase/CharacterStateBase.h"

class ActorBase;

struct Transform;

class PlayerMomentumStealAimState : public CharacterStateBase
{
public:
	PlayerMomentumStealAimState(
		float LIMIT_TIME,

		float TIME_SCALE,

		float RAYCAST_LENGTH,

		Transform& playerTrans,
		ActorBase*& target,

		const std::function<void(void)>& playAnimeAim,

		const std::function<void(void)>& changeStateSteal,
		const std::function<void(void)>& changeStateDefault
	);
	~PlayerMomentumStealAimState()override = default;

	// 状態遷移後1度行う初期化処理
	void Enter(void)override;
	// 更新処理
	void Update(void)override;
	// 状態遷移前1度行う終了処理
	void Exit(void)override;

private:

#pragma region 定数

	// 制限時間（過ぎれば不発で終了）
	const float LIMIT_TIME;

	// このステート中の時間倍率（超スローにする）
	const float TIME_SCALE;

	// レイキャストの探索可能範囲
	float RAYCAST_LENGTH;

#pragma endregion

#pragma region 参照

	// プレイヤーの回転の参照
	Transform& playerTrans;

	// ターゲット格納先
	ActorBase*& target;

	// エイム状態アニメーション再生関数
	const std::function<void(void)> playAnimeAim;

	// 実際に勢いを奪う状態へ遷移する関数
	const std::function<void(void)> changeStateSteal;
	// 通常状態へ遷移する関数
	const std::function<void(void)> changeStateDefault;

#pragma endregion

	// 制限時間計測
	float limitTimeCounter;

	// ターゲット変更
	void ChangeTarget(ActorBase* target);
};