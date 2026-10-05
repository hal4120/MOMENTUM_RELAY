#pragma once

#include "../../Common/CharacterBase/CharacterStateBase.h"

#include "../../../Common/Vector3.h"

struct Transform;

class PlayerKickDownAttackCollOperator;

class PlayerKickDownAttackState : public CharacterStateBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="COLL_START_TIME"></param>
	/// <param name="COLL_END_TIME"></param>
	/// <param name="collOperator"></param>
	/// <param name="playAnimeAttack"></param>
	/// <param name="getAnimeRatio"></param>
	/// <param name="changeStateIdle"></param>
	PlayerKickDownAttackState(
		float COLL_START_TIME,
		float COLL_END_TIME,

		PlayerKickDownAttackCollOperator& collOperator,

		std::function<void(void)> playAnimeAttack,
		std::function<float(void)> getAnimeRatio,

		std::function<void(void)> changeStateIdle
	);

	~PlayerKickDownAttackState()override = default;

	// 自分の状態に遷移する条件関数
	void OwnStateConditionUpdate(void);

	// 状態遷移後1度行う初期化処理
	void Enter(void)override;
	// 更新処理
	void Update(void)override;
	// 状態遷移前1度行う終了処理
	void Exit(void)override;

private:

#pragma region 定数

	// 攻撃ステップ
	enum class STEP
	{
		// 前隙
		Startup,

		// 攻撃判定発生中
		Active,

		// 後隙
		Recovery
	};

	// 攻撃の判定を発生させ始めるアニメーション再生割合
	const float COLL_START_TIME;
	// 攻撃の判定を発生させ終わるアニメーション再生割合
	const float COLL_END_TIME;

#pragma endregion

#pragma region 受け取る参照変数・関数

	// 攻撃の当たり判定管理クラスの参照
	PlayerKickDownAttackCollOperator& collOperator;

	// 攻撃アニメーションの再生関数のポインタ
	const std::function<void(void)> playAnimeAttack;

	// アニメーションの再生割合を取得する関数のポインタ
	const std::function<float(void)> getAnimeRatio;

	// 待機状態に戻る関数のポインタ
	const std::function<void(void)> changeStateIdle;

#pragma endregion

	// 攻撃ステップ
	STEP step;
};