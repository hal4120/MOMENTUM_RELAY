#pragma once

#include "../PostEffectBase.h"

class GrayScalePostEffect : public PostEffectBase
{
public:
	GrayScalePostEffect();
	~GrayScalePostEffect() override = default;

	void Init(void) override;
};