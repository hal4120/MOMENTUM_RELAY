#pragma once

#include "../../Common/CharacterBase/CharacterStateBase.h"

class GroundMotionController;

class PlayerJumpState : public CharacterStateBase
{
public:

	PlayerJumpState(
		GroundMotionController& motion,

		const std::function<void(void)>& playAnimeJumpStart,
		const std::function<void(void)>& playAnimeJumpLoop,
		const std::function<void(void)>& playAnimeStamp,

		const std::function<bool(void)>& isAnimeEnd,

		const std::function<void(void)>& changeStateIdle
	);

	~PlayerJumpState()override = default;

	// 自分の状態に遷移する条件関数
	void OwnStateConditionUpdate(void);

	// 状態遷移後1度行う初期化処理
	void Enter(void)override;
	// 更新処理
	void Update(void)override;

private:

#pragma region 受け取る参照変数・関数

	// 地上・空中の移動とジャンプを制御
	GroundMotionController& motion;

	// ジャンプ開始アニメーションの再生関数のポインタ
	const std::function<void(void)> playAnimeJumpStart;
	// ジャンプ滞空アニメーションの再生関数のポインタ
	const std::function<void(void)> playAnimeJumpLoop;
	// 着地アニメーションの再生関数のポインタ
	const std::function<void(void)> playAnimeStamp;

	// アニメーション終了取得関数のポインタ
	const std::function<bool(void)> isAnimeEnd;

	// 待機状態に戻る関数のポインタ
	const std::function<void(void)> changeStateIdle;

#pragma endregion

	// ステップ
	enum class STEP {

		// ジャンプ開始
		Start,

		// 滞空
		Loop,

		// 着地
		Stamp,
	};

	// 現在ステップ
	STEP step;
};