#pragma once

class ShaderBase
{
public:

    ShaderBase();

    virtual ~ShaderBase() = default;

    // シェーダーの初期化
    virtual void Init(void) {}

    // シェーダーの更新
    virtual void Update(void) {}

    // シェーダーを描画に適用
    virtual void Apply(void);

    // シェーダーの設定リセット
    virtual void ResetApply(void);

    // シェーダー固有リソースの解放
    virtual void Release(void);

protected:

    // 頂点シェーダーのハンドル
    int vertexShaderHandle;

    // ピクセルシェーダーのハンドル
    int pixelShaderHandle;
};