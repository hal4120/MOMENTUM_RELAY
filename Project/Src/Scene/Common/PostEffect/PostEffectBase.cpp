#include "PostEffectBase.h"


PostEffectBase::PostEffectBase() :
	pixelShaderHandle(-1)
{
}

// ポストエフェクトを適用
void PostEffectBase::Apply(int srcScreenHandle, int dstScreenHandle)
{
	// 描画先をポストエフェクト用Screenへ変更
	SetDrawScreen(dstScreenHandle);

	// 画面をクリア
	ClearDrawScreen();

	// 入力画像をPixelShaderへ設定
	SetUseTextureToShader(0, srcScreenHandle);

	// PixelShaderを設定
	SetUsePixelShader(pixelShaderHandle);

	// 派生PostEffect固有の設定
	ApplyParameter();

	// 画面全体へ描画
	DrawFullScreenPolygon();

	// 設定を解除
	ResetParameter();

	SetUsePixelShader(-1);

	SetUseTextureToShader(0, -1);
}

// 画面全体を覆う板ポリゴンを描画
void PostEffectBase::DrawFullScreenPolygon(void)
{
	// 描画領域のサイズを取得
	int width = 0;
	int height = 0;

	GetDrawScreenSize(&width, &height);

	// 頂点
	VERTEX2DSHADER vertex[4]{};

	// 左上
	vertex[0].pos.x = 0.0f;
	vertex[0].pos.y = 0.0f;
	vertex[0].pos.z = 0.0f;

	vertex[0].rhw = 1.0f;

	vertex[0].dif = GetColorU8(255, 255, 255, 255);

	vertex[0].u = 0.0f;
	vertex[0].v = 0.0f;


	// 右上
	vertex[1].pos.x = static_cast<float>(width);
	vertex[1].pos.y = 0.0f;
	vertex[1].pos.z = 0.0f;

	vertex[1].rhw = 1.0f;

	vertex[1].dif = GetColorU8(255, 255, 255, 255);

	vertex[1].u = 1.0f;
	vertex[1].v = 0.0f;


	// 左下
	vertex[2].pos.x = 0.0f;
	vertex[2].pos.y = static_cast<float>(height);
	vertex[2].pos.z = 0.0f;

	vertex[2].rhw = 1.0f;

	vertex[2].dif = GetColorU8(255, 255, 255, 255);

	vertex[2].u = 0.0f;
	vertex[2].v = 1.0f;


	// 右下
	vertex[3].pos.x = static_cast<float>(width);
	vertex[3].pos.y = static_cast<float>(height);
	vertex[3].pos.z = 0.0f;

	vertex[3].rhw = 1.0f;

	vertex[3].dif = GetColorU8(255, 255, 255, 255);

	vertex[3].u = 1.0f;
	vertex[3].v = 1.0f;


	// インデックス
	unsigned short index[6]
	{
		0, 1, 2,
		1, 3, 2
	};

	// 全画面描画
	DrawPolygonIndexed2DToShader(vertex, 4, index, 2);
}