#pragma once

#include "Common/ActorBase/ActorBase.h"

#include "Common/Shader/WaterShader.h"

class Water : public ActorBase
{
public:
	Water() :ActorBase() {}
	~Water()override = default;

	void Load(void)override {
		SetDynamicFlg(false);

		trans.LoadModel("Water/WaterWaveCube");

		trans.pos = Vector3(500, 100, 500);

		trans.scale = Vector3(5.0f, 0.5f, 5.0f);
	}

private:

	void SubInit(void)override {
		CreateShader(new WaterShader());
	}
};