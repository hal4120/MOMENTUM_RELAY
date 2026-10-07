#include "UnderWaterPostEffect.h"

#include "../../../../pch.h"

#include "../../../../Manager/Shader/ShaderResourceManager.h"

UnderWaterPostEffect::UnderWaterPostEffect() :
	PostEffectBase(),

	constBufferData{},
	constBufferHandle(-1)
{
	constBufferData.time = 0.0f;
	constBufferData.intensity = 1.0f;
	constBufferData.distortionStrength = 0.003f;
	constBufferData.distortionSpeed = 2.0f;
}

void UnderWaterPostEffect::Init(void)
{
	// ピクセルシェーダーを取得
	ShaderResourceManager::GetIns().CreatePixelShader(PIXEL_SHADER_TYPE::UnderWater);
	pixelShaderHandle = ShaderResourceManager::GetIns().GetPixelShader(PIXEL_SHADER_TYPE::UnderWater);

	// 定数バッファを作成
	constBufferHandle = CreateShaderConstantBuffer(sizeof(ConstBuffer));
}

void UnderWaterPostEffect::Update(void)
{
	// 時間を進める
	constBufferData.time += 1.0f / 60.0f;
}

void UnderWaterPostEffect::Release(void)
{
	if (constBufferHandle >= 0) {
		DeleteShaderConstantBuffer(constBufferHandle);
		constBufferHandle = -1;
	}
}

void UnderWaterPostEffect::ApplyParameter(void)
{
	if (constBufferHandle < 0) { return; }

	// 定数バッファの書き込み先を取得
	auto* buffer =
		static_cast<ConstBuffer*>(
			GetBufferShaderConstantBuffer(
				constBufferHandle
			)
			);

	if (buffer == nullptr) { return; }

	// CPU側のデータを書き込む
	*buffer = constBufferData;

	// GPUへ反映
	UpdateShaderConstantBuffer(constBufferHandle);

	// PixelShaderのb0へ設定
	SetShaderConstantBuffer(constBufferHandle, DX_SHADERTYPE_PIXEL, 0);
}

void UnderWaterPostEffect::ResetParameter(void)
{
	// PixelShaderのb0を解除
	SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 0);
}
