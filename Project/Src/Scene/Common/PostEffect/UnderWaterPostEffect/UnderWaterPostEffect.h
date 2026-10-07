#pragma once

#include "../PostEffectBase.h"

class UnderWaterPostEffect : public PostEffectBase
{
public:
	UnderWaterPostEffect();
	~UnderWaterPostEffect() override = default;

	void Init(void) override;
	void Update(void) override;
	void Release(void) override;

protected:

	void ApplyParameter(void) override;
	void ResetParameter(void) override;

private:

	struct ConstBuffer
	{
        // 経過時間
        float time;

        // 水中エフェクト全体の強さ
        float intensity;

        // 揺れの強さ
        float distortionStrength;

        // 揺れの速度
        float distortionSpeed;
	};

	ConstBuffer constBufferData;

    int constBufferHandle;
};