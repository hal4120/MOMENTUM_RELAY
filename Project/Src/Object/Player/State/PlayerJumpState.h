#pragma once

#include "../../Common/CharacterBase/CharacterStateBase.h"

struct Vector3;

class PlayerJumpState : public CharacterStateBase
{
public:

	PlayerJumpState(
		float JUMP_POWER,

		float& velocityY,
		const bool& isGround,

		std::function<void(const Vector3&)> moveAccel,

		std::function<void(void)> PlayAnimeJumpStart,
		std::function<void(void)> PlayAnimeJumpLoop,
		std::function<void(void)> PlayAnimeStamp,

		std::function<bool(void)> isAnimeEnd,

		std::function<void(void)> changeStateIdle
	);

	~PlayerJumpState()override = default;

	// 自分の状態に遷移する条件関数
	void OwnStateConditionUpdate(void);

	// 状態遷移後1度行う初期化処理
	void Enter(void)override;
	// 更新処理
	void Update(void)override;

private:

#pragma region 定数

	// ジャンプ力
	const float JUMP_POWER;

#pragma endregion

#pragma region 受け取る参照変数・関数

	// Y軸加速度
	float& velocityY;

	// 地面と設置しているか
	const bool& isGround;

	// 移動関数のポインタ
	const std::function<void(const Vector3&)> moveAccel;

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