#pragma once

#include "../../Common/CharacterBase/CharacterStateBase.h"

class ActorBase;

struct Transform;

class PlayerMomentumAimState : public CharacterStateBase
{
public:

	/// <summary>
	/// 勢い操作「エイム発動」（ターゲット照準選択）
	/// </summary>
	/// <param name="LIMIT_TIME">ターゲット照準選択の猶予時間（フレーム数）</param>
	/// <param name="TIME_SCALE">照準選択中のゲーム進行スピード倍率（超スロー）</param>
	/// <param name="RAYCAST_LENGTH">照準選択のレイキャストの長さ</param>
	/// <param name="playerTrans">プレイヤーのモデル制御情報の参照</param>
	/// <param name="target">ターゲット格納先</param>
	/// <param name="playAnimeAim">エイム状態アニメーション再生関数</param>
	/// <param name="changeStateMomentum">実際に勢い操作を実行する状態へ遷移する関数</param>
	/// <param name="changeStateDefault">通常状態へ遷移する関数</param>
	PlayerMomentumAimState(
		float LIMIT_TIME,

		float TIME_SCALE,

		float RAYCAST_LENGTH,

		Transform& playerTrans,
		ActorBase*& target,

		const std::function<void(void)>& playAnimeAim,

		const std::function<void(void)>& changeStateMomentum,
		const std::function<void(void)>& changeStateDefault
	);

	~PlayerMomentumAimState()override = default;

	// 状態遷移後1度行う初期化処理
	void Enter(void)override;
	// 更新処理
	void Update(void)override;
	// 状態遷移前1度行う終了処理
	void Exit(void)override;

private:

#pragma region 定数

	// 制限時間（過ぎれば不発で終了）
	const float LIMIT_TIME;

	// このステート中の時間倍率（超スローにする）
	const float TIME_SCALE;

	// レイキャストの探索可能範囲
	float RAYCAST_LENGTH;

#pragma endregion

#pragma region 参照

	// プレイヤーのモデル制御情報の参照
	Transform& playerTrans;

	// ターゲット格納先
	ActorBase*& target;

	// エイム状態アニメーション再生関数
	const std::function<void(void)> playAnimeAim;

	// 実際に勢い操作を実行する状態へ遷移する関数
	const std::function<void(void)> changeStateMomentum;
	// 通常状態へ遷移する関数
	const std::function<void(void)> changeStateDefault;

#pragma endregion

	// 制限時間計測
	float limitTimeCounter;

	// ターゲット変更
	void ChangeTarget(ActorBase* target);
};