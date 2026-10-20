#pragma once

#include "../../Common/CharacterBase/CharacterStateBase.h"

struct Vector3;
struct Quaternion;

class PlayerRollingState : public CharacterStateBase
{
public:
	
	/// <summary>
	/// 回避
	/// </summary>
	/// <param name="ACCEL_MAX">加速度制限の規定値</param>
	/// <param name="COOL_TIME">クールタイム</param>
	/// <param name="INVI_START_TIME">無敵判定発生開始時間</param>
	/// <param name="INVI_END_TIME">無敵判定発生終了時間</param>
	/// <param name="rotation">回転情報の参照</param>
	/// <param name="moveAccel">移動関数</param>
	/// <param name="accelMax">加速度制限の参照</param>
	/// <param name="isInvincible">無敵判定操作参照</param>
	/// <param name="playDodgeAnime">回避アニメーション再生関数</param>
	/// <param name="getAnimePlayRatio">アニメーション再生割合取得関数</param>
	/// <param name="defaultChangeState">回避行動終了後の状態遷移関数</param>
	PlayerRollingState(
		float ACCEL_MAX,

		int COOL_TIME,

		float INVI_START_TIME,
		float INVI_END_TIME,

		const Quaternion& rotation,

		std::function<void(const Vector3&)> moveAccel,
		float& accelMax,

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

	// デフォルト最大移動速度
	const float ACCEL_MAX;

	// 回避のクールタイム
	const int COOL_TIME;

	// 回避の無敵時間（アニメーションの再生割合）
	const float INVI_START_TIME;

	// 回避の無敵時間終了（アニメーションの再生割合）
	const float INVI_END_TIME;

#pragma	endregion 

#pragma region 受け取る参照変数・関数

	// 角度
	const Quaternion& rotation;

	// 移動関数のポインタ
	const std::function<void(const Vector3&)> moveAccel;

	// 横軸加速度の最大値の参照
	float& accelMax;

	// 回避無敵判定
	bool& isInvincible;

	// 回避アニメーションの再生関数のポインタ
	const std::function<void(void)> playDodgeAnime;
	// アニメーションの再生割合を取得する関数のポインタ
	const std::function<float(void)> getAnimePlayRatio;

	// 攻撃終了後の状態遷移関数のポインタ
	const std::function<void(void)> defaultChangeState;

#pragma endregion

	// ステップ
	STEP step;

	// クールタイム計測
	float coolTimeCounter;
};