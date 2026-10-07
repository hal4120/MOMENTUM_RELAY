#pragma once

#include "Common/ActorBase/ActorBase.h"

class SkyDome : public ActorBase
{
public:
	SkyDome() :
		ActorBase()
	{
	}
	~SkyDome()override = default;

	void Load(void)override {

		SetDynamicFlg(false);

		trans.LoadModel("SkyDome/SkyDome");

		trans.scale = 100;
	}
private:

	void SubUpdate(void)override {
		trans.AddAngleYDeg(0.01f);
		trans.Attach();
	}

};