#pragma once

#include "../../Common/CharacterBase/CharacterStateBase.h"

class PlayerIdleState : public CharacterStateBase
{
public:

	PlayerIdleState(std::function<void(void)> playAnimeIdle);

	~PlayerIdleState()override = default;

	// 自分の状態に遷移する条件関数
	void OwnStateConditionUpdate(void);

	// 状態遷移後1度行う初期化処理
	void Enter(void)override;

private:

	// 待機アニメーションの再生関数のポインタ
	const std::function<void(void)> playAnimeIdle;
};