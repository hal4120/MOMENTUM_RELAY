#pragma once

class SkyBox
{
public:

	SkyBox(float size = 1000.0f);

	~SkyBox() = default;

	// 初期化
	void Init(void);

	// 描画
	void Draw(void);

	// 解放
	void Release(void);

	// テクスチャを設定
	void SetTexture(int texture);

private:

	// 1辺の長さ
	float size;

	// 頂点バッファ
	int vertexBuffer;
	// インデックスバッファ
	int indexBuffer;

	// 頂点シェーダー
	int vertexShader;
	// ピクセルシェーダー
	int pixelShader;
	
	// テクスチャ
	int texture;
};