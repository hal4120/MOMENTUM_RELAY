#pragma once

#include "../PostEffectBase.h"

class FocusLinesPostEffect : public PostEffectBase
{
public:

	/// <summary>
	/// 集中線ポストエフェクト
	/// </summary>
	/// <param name="intensity">集中線の強さ</param>
	/// <param name="speed">集中線の変化速度</param>
	/// <param name="lineCount">集中線の本数</param>
	FocusLinesPostEffect(
		float intensity,
		float speed,
		float lineCount
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
		float speed;
		float lineCount;
	};

	// 定数バッファへ渡すデータ
	ConstBuffer constBufferData;

	// 定数バッファハンドル
	int constBufferHandle;
};