#include "TimeFreezePostEffect.h"
#include "../../../../Manager/Shader/ShaderResourceManager.h"

void TimeFreezePostEffect::Init(void)
{
    auto& shaders = ShaderResourceManager::GetIns();
    shaders.CreatePixelShader(PIXEL_SHADER_TYPE::TimeFreeze);
    pixelShaderHandle = shaders.GetPixelShader(PIXEL_SHADER_TYPE::TimeFreeze);
}
