#include "GrayScalePostEffect.h"

#include "../../../../Manager/Shader/ShaderResourceManager.h"


GrayScalePostEffect::GrayScalePostEffect() : 
	PostEffectBase()
{
}

void GrayScalePostEffect::Init(void)
{
	ShaderResourceManager::GetIns().CreatePixelShader(PIXEL_SHADER_TYPE::GrayScale);
	pixelShaderHandle = ShaderResourceManager::GetIns().GetPixelShader(PIXEL_SHADER_TYPE::GrayScale);
}