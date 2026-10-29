#pragma once
#include "../PostEffectBase.h"

class AimCrosshairPostEffect : public PostEffectBase
{
public:
    AimCrosshairPostEffect() = default;
    void Init(void) override;
};
