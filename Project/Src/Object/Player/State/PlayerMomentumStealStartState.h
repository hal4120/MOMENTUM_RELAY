#pragma once

#include "../../Common/CharacterBase/CharacterStateBase.h"

class PlayerMomentumStealStartState : public CharacterStateBase
{
public:

	/// <summary>
	/// 入力判定（単押し->「クイック発動」、長押し->「エイム発動」）
	/// </summary>
	/// <param name="COOL_TIME">クールタイム（フレーム数）</param>
	/// <param name="AIM_KEY_HOLD_TIME">長押し発動に必要な長押し時間（フレーム数）</param>
	/// <param name="changeStateQuick">「クイック発動」関数</param>
	/// <param name="changeStateAim">「エイム発動」関数</param>
	PlayerMomentumStealStartState(
		float COOL_TIME,

		float AIM_KEY_HOLD_TIME,

		const std::function<void(void)>& changeStateQuick,
		const std::function<void(void)>& changeStateAim
	);
	~PlayerMomentumStealStartState()override = default;

	// 自分の状態に遷移する条件関数
	void OwnStateConditionUpdate(void);

	// 状態遷移後1度行う初期化処理
	void Enter(void)override;
	// 更新処理
	void Update(void)override;
	// 状態遷移前1度行う終了処理
	void Exit(void)override;

	// 状態関係なく呼び出す処理
	void AlwaysUpdate(void)override;

private:

#pragma region 定数

	// クールタイム
	const float COOL_TIME;

	// 「エイム発動」になるのに必要な長押し時間
	const float AIM_KEY_HOLD_TIME;

	// 「クイック発動」関数のポインタ
	const std::function<void(void)> changeStateQuick;

	// 「エイム発動」関数のポインタ
	const std::function<void(void)> changeStateAim;

#pragma endregion

	// キーを押している時間計測
	float keyHoldCounter;

	// クールタイム計測
	float coolTimeCounter;
};