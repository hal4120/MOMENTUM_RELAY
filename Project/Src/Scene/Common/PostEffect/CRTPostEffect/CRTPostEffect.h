#pragma once

#include "../PostEffectBase.h"

class CRTPostEffect : public PostEffectBase
{
public:

	CRTPostEffect(float intensity, float distortion);

	void Init(void) override;
	void Update(void) override;
	void Release(void) override;

protected:

	void ApplyParameter(void) override;
	void ResetParameter(void) override;

private:

	struct ConstBuffer
	{
		float time;
		float intensity;
		float distortion;
		float scanLineStrength;
	};

	ConstBuffer constBufferData;

	int constBufferHandle;
};