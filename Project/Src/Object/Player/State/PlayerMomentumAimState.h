#pragma once

#include "../../Common/CharacterBase/CharacterStateBase.h"

class ActorBase;
struct Transform;

class PlayerMomentumAimState : public CharacterStateBase
{
public:
	PlayerMomentumAimState(
		float LIMIT_TIME,

		float TIME_SCALE,

		float RAYCAST_LENGTH,

		ActorBase*& target
	);
	~PlayerMomentumAimState()override = default;

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

	// ターゲット格納先
	ActorBase*& target;

	// 制限時間計測
	float limitTimeCounter;


	// ターゲット変更
	void ChangeTarget(ActorBase* target);

};