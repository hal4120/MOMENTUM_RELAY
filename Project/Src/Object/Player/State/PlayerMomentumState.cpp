#include "PlayerMomentumState.h"

#include "../Player.h"

PlayerMomentumState::PlayerMomentumState(
	ActorBase*& target,
	Vector3& stealVelocity,

	const std::function<void(void)>& playAnime,
	const std::function<float(void)>& getAnimeRatio,

	const std::function<void(void)>& changeStateDefault
) :
	target(target),
	stealVelocity(stealVelocity),

	playAnime(playAnime),
	getAnimeRatio(getAnimeRatio),

	changeStateDefault(changeStateDefault),

	step(STEP::FrontGap)
{
}

void PlayerMomentumState::Enter(void)
{
	// アニメーション再生
	playAnime();

	// ステップ初期化
	step = STEP::FrontGap;
}

void PlayerMomentumState::Update(void)
{
	// アニメーション再生割合取得
	const float animeRatio = getAnimeRatio();

	switch (step) {

	case PlayerMomentumState::STEP::FrontGap: {

		// 設定アニメーション再生割合を超えたら実行
		if (animeRatio >= getAnimeRatio()) {

			// 「実行」する
			step = STEP::Do;
		}

		break;
	}

	case PlayerMomentumState::STEP::Do: {

		// 勢いを保持している -> 勢いを「与える」行動
		if (stealVelocity != 0.0f) {

			// ターゲットに勢いを与える
			target->VelGive(stealVelocity);

			// 保持していた勢いを消去
			stealVelocity = 0.0f;
		}
		// 勢いを保持していない -> 勢いを「奪う」行動
		else {

			// ターゲットから勢いを奪うい、保持する
			stealVelocity = target->VelSteal();
		}

		// 実行終了。「後隙」にする
		step = STEP::RearGap;

		break;
	}

	case PlayerMomentumState::STEP::RearGap: {

		// アニメーションが終了していたら、通常状態に遷移
		if (animeRatio >= 1.0f) { changeStateDefault(); }

		break;
	}

	}
}