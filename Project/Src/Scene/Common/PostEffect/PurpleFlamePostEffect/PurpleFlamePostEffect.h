#pragma once

#include "../PostEffectBase.h"

class PurpleFlamePostEffect : public PostEffectBase
{
public:

	/// <summary>
	/// 画面下部から燃え上がる紫色の炎
	/// </summary>
	/// <param name="intensity">炎の強さ</param>
	/// <param name="flameHeight">炎の高さ（画面高さに対する割合）</param>
	/// <param name="flameSpeed">炎の上昇速度</param>
	PurpleFlamePostEffect(
		float intensity,
		float flameHeight,
		float flameSpeed
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
		float flameHeight;
		float flameSpeed;
	};

	ConstBuffer constBufferData;

	// 定数バッファハンドル
	int constBufferHandle;
};