#pragma once

#include "../Common/CharacterBase/CharacterBase.h"

class Player : public CharacterBase
{
public:
	Player();
	~Player()override = default;

	// 読み込み
	void Load(void)override;

	// 当たり判定の通知
	void OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)override;

private:

	// 状態定義
	enum class STATE
	{
		None = -1,

		Idle,
		Move,
		Jump,

		KickDownAttack,

		Max
	};

#pragma region アニメーション関係定義

	// アニメーションタイプ定義
	enum class ANIME_TYPE
	{
		None = -1,

		Idle,

		Walk,
		Run,

		Damage,
		Down,
		Death,
		
		KickDown,

		ForwardThrow,
		Toss,

		JumpStart,
		JumpLoop,
		Stamp,

		Max
	};

	// アニメーション再生速度テーブル
	float ANIME_SPEED_TABLE[(int)ANIME_TYPE::Max] =
	{
		1.0f,	// Idle

		1.0f,	// Walk
		1.5f,	// Run

		1.0f,	// Damage
		1.0f,	// Down
		1.0f,	// Death

		1.0f,	// KickDown

		1.0f,	// ForwardThrow
		1.0f,	// Toss

		3.0f,	// JumpStart
		1.0f,	// JumpLoop
		3.0f,	// Stamp
	};

	// アニメーションループ再生フラグテーブル
	const bool ANIME_LOOP_TABLE[(int)ANIME_TYPE::Max] =
	{
		true,	// Idle

		true,	// Walk
		true,	// Run

		false,	// Damage
		false,	// Down
		false,	// Death

		false,	// KickDown

		false,	// ForwardThrow
		false,	// Toss

		false,	// JumpStart
		true,	// JumpLoop
		false,	// Stamp
	};

#pragma endregion

	// 初期化処理
	void SubInit(void)override {

		// 待機状態に遷移
		ChangeState(STATE::Idle);
	}

	void SubUpdate(void)override;
};