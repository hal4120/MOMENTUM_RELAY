#include "GoalDirectionPostEffect.h"

#include <stdexcept>

#include "../../../../pch.h"

#include "../../../../Manager/Shader/ShaderResourceManager.h"

GoalDirectionPostEffect::GoalDirectionPostEffect() :
    PostEffectBase(),
    constBufferData{},
    constBufferHandle(-1),
    textTextureHandle(-1),
    finished(false)
{
    constBufferData.time = 0.0f;
    constBufferData.intensity = 1.0f;
    constBufferData.flashSpeed = 2.6f;
    constBufferData.padding = 0.0f;
}

void GoalDirectionPostEffect::Init(void)
{
    finished = false;
    constBufferData.time = 0.0f;

    ShaderResourceManager::GetIns().CreatePixelShader(PIXEL_SHADER_TYPE::GoalDirection);

    pixelShaderHandle = ShaderResourceManager::GetIns().GetPixelShader(PIXEL_SHADER_TYPE::GoalDirection);

    textTextureHandle = LoadGraph("Data/Image/Effect/GoalDirectionText.png");
    if (textTextureHandle < 0) {
        throw std::runtime_error(
            "GoalDirectionText.png ‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s‚µ‚Ü‚µ‚½"
        );
    }

    constBufferHandle = CreateShaderConstantBuffer(sizeof(ConstBuffer));
}

void GoalDirectionPostEffect::Update(void)
{
    if (!finished) { constBufferData.time += 1.0f / 60.0f; }
}

void GoalDirectionPostEffect::Release(void)
{
    if (constBufferHandle != -1) { DeleteShaderConstantBuffer(constBufferHandle); constBufferHandle = -1; }
    if (textTextureHandle != -1) { DeleteGraph(textTextureHandle); textTextureHandle = -1; }
    pixelShaderHandle = -1;
}

void GoalDirectionPostEffect::ApplyParameter(void)
{
    if (constBufferHandle == -1) { return; }

    SetUseTextureToShader(1, textTextureHandle);

    auto* buffer = static_cast<ConstBuffer*>(GetBufferShaderConstantBuffer(constBufferHandle));

    if (!buffer) { return; }

    *buffer = constBufferData;
    UpdateShaderConstantBuffer(constBufferHandle);
    SetShaderConstantBuffer(constBufferHandle, DX_SHADERTYPE_PIXEL, 0);
}

void GoalDirectionPostEffect::ResetParameter(void)
{
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 0);
    SetUseTextureToShader(1, -1);
}
