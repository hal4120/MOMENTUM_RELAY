#pragma once

#include "../PostEffectBase.h"

class DamageFlashPostEffect : public PostEffectBase
{
public:
    /// <summary>
    /// ダメージ
    /// </summary>
    /// <param name="intensity">最大強度</param>
    /// <param name="edgeStrength">画面端の強調</param>
    /// <param name="duration">秒数</param>
    DamageFlashPostEffect(float intensity, float edgeStrength, float duration = 60.0f);

    void Init(void) override;
    void Update(void) override;
    void Release(void) override;
    bool IsFinished(void) const override;

protected:
    void ApplyParameter(void) override;
    void ResetParameter(void) override;

private:
    struct ConstBuffer
    {
        float intensity;
        float edgeStrength;
        float padding[2];
    };

    ConstBuffer constBufferData;
    int constBufferHandle;

    float initialIntensity;
    float duration;
    float elapsedTime;
};
