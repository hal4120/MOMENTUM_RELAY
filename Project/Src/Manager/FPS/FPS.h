#pragma once

static constexpr float FRAME_RATE(1000 / 60);

class FPS
{
public:
	FPS();
	~FPS();

	//èâä˙âª
	void Init(void);

	bool UpdateFrameRate(void);

	void CalcFrameRate(void);

	void DrawFrameRate(void);

private:
	int currentTime;
	int prevFrameTime;

	int frameCnt;
	int updateFrameRateTime;

	float frameRate;
};

