#include "PurpleFlamePostEffect.h"

#include "../../../../pch.h"

#include "../../../../Manager/Shader/ShaderResourceManager.h"


PurpleFlamePostEffect::PurpleFlamePostEffect(
	float intensity,
	float flameHeight,
	float flameSpeed
) :
	constBufferData{},
	constBufferHandle(-1)
{
	constBufferData.time = 0.0f;
	constBufferData.intensity = intensity;
	constBufferData.flameHeight = flameHeight;
	constBufferData.flameSpeed = flameSpeed;
}


// 初期化
void PurpleFlamePostEffect::Init(void)
{
	ShaderResourceManager::GetIns().CreatePixelShader(PIXEL_SHADER_TYPE::PurpleFlame);

	// ピクセルシェーダーを取得
	// ※ ShaderResourceManager側で事前ロードされている想定
	pixelShaderHandle =
		ShaderResourceManager::GetIns().GetPixelShader(
			PIXEL_SHADER_TYPE::PurpleFlame
		);

	// 定数バッファを作成
	constBufferHandle =
		CreateShaderConstantBuffer(
			sizeof(ConstBuffer)
		);
}


// 更新
void PurpleFlamePostEffect::Update(void)
{
	// 時間を進める
	constBufferData.time += 1.0f / 60.0f;
}


// 描画直前の固有設定
void PurpleFlamePostEffect::ApplyParameter(void)
{
	if (constBufferHandle < 0)
	{
		return;
	}

	// 定数バッファの書き込み先を取得
	auto* buffer =
		static_cast<ConstBuffer*>(
			GetBufferShaderConstantBuffer(
				constBufferHandle
			)
			);

	if (buffer == nullptr)
	{
		return;
	}

	// CPU側のデータを書き込む
	*buffer = constBufferData;

	// GPU側へ反映
	UpdateShaderConstantBuffer(
		constBufferHandle
	);

	// PixelShaderのb0へ設定
	SetShaderConstantBuffer(
		constBufferHandle,
		DX_SHADERTYPE_PIXEL,
		0
	);
}


// 描画後の固有設定リセット
void PurpleFlamePostEffect::ResetParameter(void)
{
	// PixelShaderのb0を解除
	SetShaderConstantBuffer(
		-1,
		DX_SHADERTYPE_PIXEL,
		0
	);
}


// 解放
void PurpleFlamePostEffect::Release(void)
{
	if (constBufferHandle >= 0)
	{
		DeleteShaderConstantBuffer(
			constBufferHandle
		);

		constBufferHandle = -1;
	}

	// pixelShaderHandleはShaderResourceManager所有なので
	// ここではDeleteShaderしない
}