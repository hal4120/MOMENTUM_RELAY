#pragma once

#include "../Common/ActorBase/ActorBase.h"

class GoalArea : public ActorBase
{
public:
	GoalArea();
	~GoalArea()override = default;

	void Load(void)override;

private:

	void DefaultCreateShader(void)override;

	// デフォルト描画タイプ設定
	ACTOR_DRAW_TYPE DefaultDrawType(void)const override { return ACTOR_DRAW_TYPE::Additive; }
};