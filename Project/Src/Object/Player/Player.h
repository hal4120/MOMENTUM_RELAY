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

	char OnIcePostEffectSwitch(void) {
		if (prevIsOnIce == nowIsOnIce) { return -1; }
		return (char)nowIsOnIce;
	}

private:

	// 状態定義
	enum class STATE
	{
		None = -1,

		Idle,
		Move,
		Jump,

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

		JumpStart,
		JumpLoop,
		Stamp,

		Max
	};

	// アニメーション再生速度テーブル
	float ANIME_SPEED_TABLE[(int)ANIME_TYPE::Max] =
	{
		0.5f,	// Idle

		0.75f,	// Walk
		1.25f,	// Run

		2.0f,	// JumpStart
		0.5f,	// JumpLoop
		2.5f,	// Stamp
	};

	// アニメーションループ再生フラグテーブル
	const bool ANIME_LOOP_TABLE[(int)ANIME_TYPE::Max] =
	{
		true,	// Idle

		true,	// Walk
		true,	// Run

		false,	// JumpStart
		true,	// JumpLoop
		false,	// Stamp
	};

#pragma endregion

	// 初期化処理
	void SubInit(void)override {

		// 待機状態に遷移
		ChangeState(STATE::Idle);

		ACCEL_RATE = 3.0f;
		DECEL_RATE = 3.0f;

		ACCEL_MAX = 40.0f;
	}

	void SubUpdate(void)override;

	void SubOnGrounded(COLLIDER_TAG ownTag, const ColliderBase& other)override;

	bool nowIsOnIce = false;
	bool prevIsOnIce = false;
};