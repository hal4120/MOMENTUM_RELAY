#pragma once

#include "../../SceneBase.h"

class FadeSceneBase : public SceneBase
{
public:
	enum class STATE
	{
		FadeOut,
		Wait,
		FadeIn,
		End,
	};

	FadeSceneBase(unsigned short FADE_TIME = 30);
	virtual ~FadeSceneBase(void)override = default;

	bool IsFadeOutEnd(void)const { return state == STATE::Wait; }
	void StartFadeIn(void);
	bool IsEnd(void)const { return state == STATE::End; }
	STATE GetFadeState(void)const { return state; }
	float GetFadeRatio(void)const;

	bool IsUseLoadingScreen(void)const override { return false; }

protected:
	virtual void SubFadeUpdate(void) {}
	virtual void SubFadeDraw(float ratio) = 0;

private:
	void SubPostUpdate(void)override;
	void SubUiDraw(void)override;

	unsigned short fadeTime;
	unsigned short fadeCounter;
	STATE state;
};
