#pragma once

#include "../../Common/CharacterBase/CharacterStateBase.h"

class ActorBase;

struct Vector3;

class PlayerMomentumState : public CharacterStateBase
{
public:
	PlayerMomentumState(
		ActorBase*& target,
		Vector3& stealVelocity,
		ActorBase*& stealSource,

		const std::function<void(void)>& playAnime,
		const std::function<float(void)>& getAnimeRatio,

		const std::function<void(void)>& changeStateDefault
	);

	~PlayerMomentumState()override = default;

	// 状態遷移後1度行う初期化処理
	void Enter(void)override;
	// 更新処理
	void Update(void)override;

private:

#pragma region 定数

	// ステップ
	enum class STEP
	{
		// 前隙
		FrontGap,

		// 実行
		Do,

		// 後隙
		RearGap
	};

	// 実行するアニメーション再生割合
	const float DO_TIME = 0.75f;

#pragma endregion

	// ターゲット格納先
	ActorBase*& target;

	// 奪って保持している勢い
	Vector3& stealVelocity;
	// 奪って保持している勢いの元の持ち主
	ActorBase*& stealSource;

	// アニメーション再生関数
	std::function<void(void)> playAnime;

	// アニメーション再生割合取得関数
	std::function<float(void)> getAnimeRatio;

	// 通常状態へ遷移する関数
	const std::function<void(void)> changeStateDefault;


	// ステップ
	STEP step;
};