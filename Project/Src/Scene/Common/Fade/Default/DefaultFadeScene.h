#pragma once

#include "../FadeSceneBase.h"

class DefaultFadeScene : public FadeSceneBase
{
public:
	DefaultFadeScene(unsigned short FADE_TIME = 30,
		unsigned int FADE_OUT_COLOR = 0x000000,
		unsigned int FADE_IN_COLOR = 0x000000);
	~DefaultFadeScene(void)override = default;

private:
	void SubFadeDraw(float ratio)override;

	unsigned int fadeOutColor;
	unsigned int fadeInColor;
};
