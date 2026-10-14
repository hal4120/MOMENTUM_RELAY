#include "MenacingPostEffect.h"

#include "../../../../pch.h"

#include "../../../../Manager/Shader/ShaderResourceManager.h"


MenacingPostEffect::MenacingPostEffect(
	float intensity,
	float shakeStrength,
	float moveSpeed
) :
	constBufferData{},
	constBufferHandle(-1),
	glyphHandle(-1)
{
	constBufferData.time = 0.0f;
	constBufferData.intensity = intensity;
	constBufferData.shakeStrength = shakeStrength;
	constBufferData.moveSpeed = moveSpeed;
}


// 初期化
void MenacingPostEffect::Init(void)
{
	// ピクセルシェーダーを作成
	ShaderResourceManager::GetIns().CreatePixelShader(
		PIXEL_SHADER_TYPE::Menacing
	);

	// ピクセルシェーダーを取得
	pixelShaderHandle =
		ShaderResourceManager::GetIns().GetPixelShader(
			PIXEL_SHADER_TYPE::Menacing
		);

	// 定数バッファを作成
	constBufferHandle =
		CreateShaderConstantBuffer(
			sizeof(ConstBuffer)
		);

	// 「ゴ」の画像を読み込む
	glyphHandle = LoadGraph(
		"Data/Image/Effect/Go.png"
	);
}


// 更新
void MenacingPostEffect::Update(void)
{
	// 時間を進める
	constBufferData.time += 1.0f / 60.0f;
}


// 描画直前の固有設定
void MenacingPostEffect::ApplyParameter(void)
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

	// 「ゴ」の画像をt1へ設定
	SetUseTextureToShader(
		1,
		glyphHandle
	);
}


// 描画後の固有設定リセット
void MenacingPostEffect::ResetParameter(void)
{
	// 定数バッファを解除
	SetShaderConstantBuffer(
		-1,
		DX_SHADERTYPE_PIXEL,
		0
	);

	// 「ゴ」の画像を解除
	SetUseTextureToShader(
		1,
		-1
	);
}


// 解放
void MenacingPostEffect::Release(void)
{
	// 定数バッファを解放
	if (constBufferHandle >= 0)
	{
		DeleteShaderConstantBuffer(
			constBufferHandle
		);

		constBufferHandle = -1;
	}

	// 画像を解放
	if (glyphHandle >= 0)
	{
		DeleteGraph(
			glyphHandle
		);

		glyphHandle = -1;
	}

	// ピクセルシェーダーはShaderResourceManagerが所有するため解放しない
}