#pragma once

#include "../../Common/CharacterBase/CharacterStateBase.h"

struct Quaternion;

class GroundMotionController;

class PlayerRollingState : public CharacterStateBase
{
public:
	
	/// <summary>
	/// 回避
	/// </summary>
	/// <param name="ROLL_SPEED">回避時に付与する水平速度</param>
	/// <param name="ROLL_LIFT_SPEED">回避開始時の上方向速度（接地中のみ）</param>
	/// <param name="COOL_TIME">回避のクールタイム</param>
	/// <param name="INVI_START_TIME">回避の無敵時間（アニメーションの再生割合）</param>
	/// <param name="INVI_END_TIME">回避の無敵時間終了（アニメーションの再生割合）</param>
	/// <param name="rotation">角度</param>
	/// <param name="motion">地上移動Controller</param>
	/// <param name="isInvincible">回避無敵判定</param>
	/// <param name="playDodgeAnime">回避アニメーションの再生関数のポインタ</param>
	/// <param name="getAnimePlayRatio">アニメーションの再生割合を取得する関数のポインタ</param>
	/// <param name="defaultChangeState">回避終了後の状態遷移関数のポインタ</param>
	PlayerRollingState(
		float ROLL_SPEED,
		float ROLL_LIFT_SPEED,

		float COOL_TIME,

		float INVI_START_TIME,
		float INVI_END_TIME,

		const Quaternion& rotation,

		GroundMotionController& motion,

		bool& isInvincible,

		std::function<void(void)> playDodgeAnime,
		std::function<float(void)> getAnimePlayRatio,
		std::function<void(void)> defaultChangeState
	);

	~PlayerRollingState()override = default;

	// 自分の状態に遷移する条件関数
	void OwnStateConditionUpdate(void)override;

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

	// ステップ
	enum class STEP
	{
		// 前隙
		Startup,

		// 無敵判定発生中
		Active,

		// 後隙
		Recovery
	};

	// 回避時に付与する水平速度
	const float ROLL_SPEED;

	// 回避開始時の上方向速度（接地中のみ）
	const float ROLL_LIFT_SPEED;

	// 回避のクールタイム
	const float COOL_TIME;

	// 回避の無敵時間（アニメーションの再生割合）
	const float INVI_START_TIME;
	// 回避の無敵時間終了（アニメーションの再生割合）
	const float INVI_END_TIME;

#pragma	endregion 

#pragma region 受け取る参照変数・関数

	// 角度
	const Quaternion& rotation;

	// 地上移動Controller
	GroundMotionController& motion;

	// 回避無敵判定
	bool& isInvincible;

	// 回避アニメーションの再生関数のポインタ
	const std::function<void(void)> playDodgeAnime;

	// アニメーションの再生割合を取得する関数のポインタ
	const std::function<float(void)> getAnimePlayRatio;

	// 回避終了後の状態遷移関数のポインタ
	const std::function<void(void)> defaultChangeState;

#pragma endregion

	// ステップ
	STEP step;

	// クールタイム計測
	float coolTimeCounter;
};