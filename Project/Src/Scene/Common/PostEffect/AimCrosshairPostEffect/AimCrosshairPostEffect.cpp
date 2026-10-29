#include "AimCrosshairPostEffect.h"
#include "../../../../Manager/Shader/ShaderResourceManager.h"

void AimCrosshairPostEffect::Init(void)
{
    auto& shaders = ShaderResourceManager::GetIns();
    shaders.CreatePixelShader(PIXEL_SHADER_TYPE::AimCrosshair);
    pixelShaderHandle = shaders.GetPixelShader(PIXEL_SHADER_TYPE::AimCrosshair);
}
