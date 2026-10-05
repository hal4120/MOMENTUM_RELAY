#include"FPS.h"

#include "../../pch.h"

#include"../../Application/Application.h"
#include"../../Scene/SceneManager.h"


FPS::FPS()
	:currentTime(0)
	,prevFrameTime(0)
	,frameCnt(0)
	,updateFrameRateTime(0)
	,frameRate(0.0f)
{
}

FPS::~FPS()
{
}

void FPS::Init(void)
{
	currentTime = 0;
	prevFrameTime = 0;
	frameCnt = 0;
	updateFrameRateTime = 0;
	frameRate = 0.0f;
}

bool FPS::UpdateFrameRate(void)
{
	Sleep(1);	//ƒVƒXƒeƒ€‚Éˆ—‚ð•Ô‚·

	currentTime = GetNowCount();


	if (currentTime - prevFrameTime >= FRAME_RATE)
	{
		prevFrameTime = currentTime;

		frameCnt++;

		return true;
	}

	return false;
}

void FPS::CalcFrameRate(void)
{

	int difTime = currentTime - updateFrameRateTime;


	if (difTime > 1000)
	{
		float castFrameCnt = (float)(frameCnt * 1000);

		frameRate = castFrameCnt / difTime;

		frameCnt = 0;

		updateFrameRateTime = currentTime;
	}
}

void FPS::DrawFrameRate(void)
{
	DrawFormatString(
		Application::SCREEN_SIZE_X - 90,
		Application::SCREEN_SIZE_Y - 20,
		0xff0000,
		"FPS[%.2f]",
		frameRate
		);
}
