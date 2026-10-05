#include "DefaultFadeScene.h"

#include "../../../../pch.h"

#include "../../../../Application/Application.h"

DefaultFadeScene::DefaultFadeScene(unsigned short FADE_TIME, unsigned int FADE_OUT_COLOR, unsigned int FADE_IN_COLOR) :
	FadeSceneBase(FADE_TIME),
	fadeOutColor(FADE_OUT_COLOR),
	fadeInColor(FADE_IN_COLOR)
{
}

void DefaultFadeScene::SubFadeDraw(float ratio)
{
	const unsigned int color =
		(GetFadeState() == STATE::FadeIn) ? fadeInColor : fadeOutColor;
	const int alpha = static_cast<int>(255.0f * ratio);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, App::SCREEN_SIZE_X, App::SCREEN_SIZE_Y, color, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
