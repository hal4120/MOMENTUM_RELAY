#include "DamageFlashPostEffect.h"

#include <algorithm>

#include "../../../../pch.h"

#include "../../../../Manager/Shader/ShaderResourceManager.h"

#include "../../../../Manager/TimeScale/TimeScale.h"

DamageFlashPostEffect::DamageFlashPostEffect(
    float intensity,
    float edgeStrength,
    float duration
) :
    constBufferData{},
    constBufferHandle(-1),
    initialIntensity((std::max)(0.0f, intensity)),
    duration((std::max)(0.0f, duration)),
    elapsedTime(0.0f)
{
    constBufferData.intensity = initialIntensity;
    constBufferData.edgeStrength = edgeStrength;
}

void DamageFlashPostEffect::Init(void)
{
    ShaderResourceManager::GetIns().CreatePixelShader(PIXEL_SHADER_TYPE::DamageFlash);

    // ShaderResourceManager側で事前にロードしておく
    pixelShaderHandle = ShaderResourceManager::GetIns().GetPixelShader(
        PIXEL_SHADER_TYPE::DamageFlash
    );

    // DxLibの非同期ロード終了後に実行すること
    constBufferHandle = CreateShaderConstantBuffer(sizeof(ConstBuffer));
}

void DamageFlashPostEffect::Update(void)
{
    elapsedTime += TimeScale::Get();

    const float rate = (duration > 0.0f) ? (std::min)(elapsedTime / duration, 1.0f) : 1.0f;

    // 徐々に減衰するフラッシュ
    const float fade = 1.0f - rate;
    constBufferData.intensity = initialIntensity * fade * fade;
}

bool DamageFlashPostEffect::IsFinished(void) const
{
    return elapsedTime >= duration;
}

void DamageFlashPostEffect::ApplyParameter(void)
{
    if (constBufferHandle < 0) { return; }

    auto* buffer = static_cast<ConstBuffer*>(GetBufferShaderConstantBuffer(constBufferHandle));
    if (buffer == nullptr) { return; }

    *buffer = constBufferData;
    UpdateShaderConstantBuffer(constBufferHandle);
    SetShaderConstantBuffer(constBufferHandle, DX_SHADERTYPE_PIXEL, 0);
}

void DamageFlashPostEffect::ResetParameter(void)
{
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 0);
}

void DamageFlashPostEffect::Release(void)
{
    if (constBufferHandle >= 0)
    {
        DeleteShaderConstantBuffer(constBufferHandle);
        constBufferHandle = -1;
    }
}
