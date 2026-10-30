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

		if (nowIsOnIce) { DECEL_RATE = 0.05f; }
		else { DECEL_RATE = 3.0f; }

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

		Attack,

		Rolling,

		MomentumInput,
		MomentumQuick,
		MomentumAim,
		Momentum,

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
		JumpEnd,

		Totta,

		Rolling,

		MomentumStealAim,

		Swim,

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
		2.5f,	// JumpEnd,

		5.0f,	// Totta

		2.5f,	// Rolling
		
		1.0f,	// MomentumStealAim

		1.0f,	// Swim
	};

	// アニメーションループ再生フラグテーブル
	const bool ANIME_LOOP_TABLE[(int)ANIME_TYPE::Max] =
	{
		true,	// Idle

		true,	// Walk
		true,	// Run

		false,	// JumpStart
		true,	// JumpLoop
		false,	// JumpEnd

		false,	// Totta
		
		false,	// Rolling

		true,	// MomentumStealAim

		true,	// Swim
	};

	// アニメーションファイルパス
	const std::string ANIME_FILE_PATH = "Data/Model/Player/Animation/";

	// アニメーションの名前テーブル
	const std::string ANIME_NAME_TABLE[(int)ANIME_TYPE::Max] =
	{
		"Idle",				// Idle
		
		"Walk",				// Walk
		"Run",				// Run

		"JumpStart",		// JumpStart
		"JumpLoop",			// JumpLoop
		"JumpEnd",			// JumpEnd

		"Totta",			// Totta

		"Rolling",			// Rolling
		
		"MomentumStealAim",	// MomentumStealAim
		
		"Swim",				// Swim
	};

	// アニメーションファイル拡張子
	const std::string ANIME_FILE_EXTENSION = ".mv1";

#pragma endregion

	// デフォルトシェーダー
	void DefaultCreateShader(void);

	// 初期化処理
	void SubInit(void)override {

		// 待機状態に遷移
		ChangeState(STATE::Idle);

		ACCEL_RATE = 3.0f;
		DECEL_RATE = 3.0f;

		ACCEL_MAX = 40.0f;
	}

	void SubUpdate(void)override;

	void SubOnPushed(COLLIDER_TAG ownTag, const ColliderBase& other, const Vector3& pushVec)override;

	bool nowIsOnIce = false;
	bool prevIsOnIce = false;

	// 回避無敵判定
	bool isInvincble = false;

	// 勢い操作を実行するターゲット
	ActorBase* target;

	// 奪って保持している勢い
	Vector3 stealVelocity;
	// 奪って保持している勢いの元の持ち主
	ActorBase* stealSource;

	// 今勢いを保持しているか（true = している、false = していない）
	bool IsSteal(void)const { return (stealVelocity != 0.0f && stealSource != nullptr); }
};