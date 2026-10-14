#pragma once

#include "../PostEffectBase.h"

class MenacingPostEffect : public PostEffectBase
{
public:

	/// <summary>
	/// 「ゴゴゴ」演出用ポストエフェクト
	/// </summary>
	/// <param name="intensity">文字の表示強度（0.0～1.0）</param>
	/// <param name="shakeStrength">文字の震えの強さ</param>
	/// <param name="moveSpeed">文字の移動速度</param>
	MenacingPostEffect(
		float intensity,
		float shakeStrength,
		float moveSpeed
	);

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 解放
	void Release(void) override;

protected:

	// 描画直前の固有設定
	void ApplyParameter(void) override;

	// 描画後の固有設定リセット
	void ResetParameter(void) override;

private:

	// ピクセルシェーダー用定数バッファ
	struct ConstBuffer
	{
		float time;
		float intensity;
		float shakeStrength;
		float moveSpeed;
	};

	// 定数バッファへ渡すデータ
	ConstBuffer constBufferData;

	// 定数バッファハンドル
	int constBufferHandle;

	// 「ゴ」の画像ハンドル
	int glyphHandle;
};