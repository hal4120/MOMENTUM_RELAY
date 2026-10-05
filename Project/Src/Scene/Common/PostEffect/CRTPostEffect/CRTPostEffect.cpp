#include "CRTPostEffect.h"

#include "../../../../pch.h"

#include "../../../../Manager/Shader/ShaderResourceManager.h"


CRTPostEffect::CRTPostEffect(float intensity, float distortion) :
	constBufferData{},
	constBufferHandle(-1)
{
	constBufferData.time = 0.0f;
	constBufferData.intensity = intensity;
	constBufferData.distortion = distortion;
	constBufferData.scanLineStrength = 0.15f;
}


// 初期化
void CRTPostEffect::Init(void)
{
	// ピクセルシェーダーを取得
	// ※ここは現在のShaderResourceManagerの取得方法に合わせる
	ShaderResourceManager::GetIns().CreatePixelShader(PIXEL_SHADER_TYPE::CRT);
	pixelShaderHandle = ShaderResourceManager::GetIns().GetPixelShader(PIXEL_SHADER_TYPE::CRT);

	// 定数バッファを作成
	constBufferHandle = CreateShaderConstantBuffer(sizeof(ConstBuffer));
}

// 更新
void CRTPostEffect::Update(void)
{
	// 時間を進める
	constBufferData.time += 1.0f / 60.0f;
}

// 固有パラメータを適用
void CRTPostEffect::ApplyParameter(void)
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


// 固有パラメータをリセット
void CRTPostEffect::ResetParameter(void)
{
	// PixelShaderのb0を解除
	SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 0);
}


// 解放
void CRTPostEffect::Release(void)
{
	if (constBufferHandle >= 0) {
		DeleteShaderConstantBuffer(constBufferHandle);
		constBufferHandle = -1;
	}

	// pixelShaderHandleはShaderResourceManager所有なので
	// ここではDeleteShaderしない
}