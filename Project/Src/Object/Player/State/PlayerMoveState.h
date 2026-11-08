#pragma once

#include "../../Common/CharacterBase/CharacterStateBase.h"

struct Vector3;
class GroundMotionController;

class PlayerMoveState : public CharacterStateBase
{
public:

	PlayerMoveState(
		float MOVE_SPEED, float DASH_SPEED_RATE, float DASH_STAMINA_MAX,
		GroundMotionController& motion,
		std::function<void(void)> playAnimeWalk,
		std::function<void(void)> playAnimeRun
	);

	~PlayerMoveState()override = default;

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

	// スタミナを返す関数
	const float& GetDashStamina(void)const { return dashStamina; }
	// 息切れかどうかを返す関数
	bool IsTired(void)const { return isTired; }

private:

#pragma region 定数

	// 非ダッシュ時の最大移動速度
	const float MOVE_SPEED;

	// ダッシュの移動量倍率
	const float DASH_SPEED_RATE;

	// ダッシュのスタミナの最大量（1フレームずつデクリメント）
	const float DASH_STAMINA_MAX;

#pragma endregion

#pragma region 受け取る参照変数・関数

	// 移動関数のポインタ
	GroundMotionController& motion;

	// 歩きアニメーションの再生関数のポインタ
	const std::function<void(void)> playAnimeWalk;
	// 走りアニメーションの再生関数のポインタ
	const std::function<void(void)> playAnimeRun;

#pragma endregion

	// ダッシュしているかどうか
	bool isDash;

	// ダッシュスタミナ
	float dashStamina;

	// 息切れ(1度スタミナが0になったかどうか)
	bool isTired;

	// 移動方向入力取得
	Vector3 InputVec(void)const;
};