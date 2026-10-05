#pragma once

#include "ShaderBase.h"

class WaterShader : public ShaderBase
{
public:
	WaterShader();
	~WaterShader()override = default;

	// シェーダーの初期化
	void Init(void)override;

	// シェーダーの更新
	void Update(void)override;

	// シェーダーを描画に適用
	void Apply(void)override;

	// シェーダーの設定リセット
	void ResetApply(void)override;

	// シェーダー固有リソースの解放
	void Release(void)override;

private:

	// 頂点シェーダーの定数バッファ構造体定義
	struct VertexShaderConstBuffer
	{
		float uvScale[2];
		float time;
		float waveHeight;

		float waveLength;
		float waveSpeed;
		float waterPadding[2];
	};
	// 頂点シェーダーの定数バッファ構造体の実際のデータ
	VertexShaderConstBuffer vertexShaderConstBufferData;

	// 頂点シェーダーの定数バッファハンドル
	int vertexShaderConstBufferHandle;

	// ピクセルシェーダーの定数バッファ構造体定義
	struct PixelShaderConstBuffer
	{
		float uvScrollSpeed[2];
		float time;
		float alpha;
	};
	// ピクセルシェーダーの定数バッファ構造体の実際のデータ
	PixelShaderConstBuffer pixelShaderConstBufferData;

	// ピクセルシェーダーの定数バッファハンドル
	int pixelShaderConstBufferHandle;
};