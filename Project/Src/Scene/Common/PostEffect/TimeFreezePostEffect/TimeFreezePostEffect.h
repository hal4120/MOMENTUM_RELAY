#pragma once
#include "../PostEffectBase.h"

class TimeFreezePostEffect : public PostEffectBase
{
public:
    TimeFreezePostEffect() = default;
    void Init(void) override;
};
