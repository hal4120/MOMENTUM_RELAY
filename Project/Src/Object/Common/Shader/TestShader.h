#pragma once

#include "ShaderBase.h"

class TestShader : public ShaderBase
{
public:

    TestShader();

    void Init(void) override;
    void Update(void) override;
    void Apply(void) override;
    void ResetApply(void) override;
    void Release(void) override;

private:

    struct PARAM
    {
        float time;
        float scale;
        float intensity;
        float padding;
    };

    PARAM param;

    int constantBufferHandle;
};