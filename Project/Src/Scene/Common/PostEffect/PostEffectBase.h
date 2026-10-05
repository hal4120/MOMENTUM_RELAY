#pragma once

#include "../../../pch.h"

class PostEffectBase
{
public:

	PostEffectBase();
	virtual ~PostEffectBase() = default;

	// ポストエフェクトの初期化
	virtual void Init(void) {}

	// ポストエフェクトの更新
	virtual void Update(void) {}

	// ポストエフェクトを適用
	void Apply(int srcScreenHandle, int dstScreenHandle);

	// ポストエフェクト固有リソースの解放
	virtual void Release(void) {}

protected:

	// 描画直前の固有設定
	// 定数バッファ等を設定する場合に派生先で再定義
	virtual void ApplyParameter(void) {}

	// 描画後の固有設定リセット
	virtual void ResetParameter(void) {}

protected:

	// 使用するピクセルシェーダーハンドル
	int pixelShaderHandle;

private:

	// 画面全体を覆う板ポリゴンを描画
	void DrawFullScreenPolygon(void);
};