#include "FadeSceneBase.h"

#include <algorithm>

FadeSceneBase::FadeSceneBase(unsigned short FADE_TIME) : SceneBase(),
	fadeTime((std::max)(static_cast<unsigned short>(1), FADE_TIME)),
	fadeCounter(0),
	state(STATE::FadeOut)
{
}

void FadeSceneBase::StartFadeIn(void)
{
	if (state != STATE::Wait) { return; }

	fadeCounter = fadeTime;
	state = STATE::FadeIn;
}

float FadeSceneBase::GetFadeRatio(void)const
{
	if (state == STATE::Wait) { return 1.0f; }
	if (state == STATE::End) { return 0.0f; }

	return static_cast<float>(fadeCounter) / static_cast<float>(fadeTime);
}

void FadeSceneBase::SubPostUpdate(void)
{
	SubFadeUpdate();

	switch (state)
	{
	case STATE::FadeOut:
		if (++fadeCounter >= fadeTime)
		{
			fadeCounter = fadeTime;
			state = STATE::Wait;
		}
		break;

	case STATE::Wait:
		break;

	case STATE::FadeIn:
		if (fadeCounter > 0) { fadeCounter--; }
		if (fadeCounter == 0) { state = STATE::End; }
		break;

	case STATE::End:
	default:
		break;
	}
}

void FadeSceneBase::SubUiDraw(void)
{
	SubFadeDraw(GetFadeRatio());
}
