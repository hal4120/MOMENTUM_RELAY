#include "WaterShader.h"

#include "../../../pch.h"

#include "../../../Manager/Shader/ShaderResourceManager.h"

#include "../../../Manager/TimeScale/TimeScale.h"

WaterShader::WaterShader() :
    vertexShaderConstBufferData{},
    vertexShaderConstBufferHandle(-1),

    pixelShaderConstBufferData{},
    pixelShaderConstBufferHandle(-1)
{
}


// シェーダーの初期化
void WaterShader::Init(void)
{
    ShaderResourceManager::GetIns().CreateVertexShader(VERTEX_SHADER_TYPE::Water);
    ShaderResourceManager::GetIns().CreatePixelShader(PIXEL_SHADER_TYPE::Water);

    // Shaderの取得
    vertexShaderHandle =
        ShaderResourceManager::GetIns().GetVertexShader(VERTEX_SHADER_TYPE::Water);

    pixelShaderHandle =
        ShaderResourceManager::GetIns().GetPixelShader(PIXEL_SHADER_TYPE::Water);


    // 頂点シェーダー定数バッファ

    vertexShaderConstBufferHandle = CreateShaderConstantBuffer(sizeof(VertexShaderConstBuffer));

    // 初期値
    vertexShaderConstBufferData.uvScale[0] = 1.0f;
    vertexShaderConstBufferData.uvScale[1] = 1.0f;

    vertexShaderConstBufferData.time = 0.0f;

    vertexShaderConstBufferData.waveHeight = 5.0f;
    vertexShaderConstBufferData.waveLength = 100.0f;
    vertexShaderConstBufferData.waveSpeed = 10.0f;

    vertexShaderConstBufferData.waterPadding[0] = 0.0f;
    vertexShaderConstBufferData.waterPadding[1] = 0.0f;


    // ピクセルシェーダー定数バッファ
    pixelShaderConstBufferHandle = CreateShaderConstantBuffer(sizeof(PixelShaderConstBuffer));

    // 初期値
    pixelShaderConstBufferData.uvScrollSpeed[0] = 0.02f;
    pixelShaderConstBufferData.uvScrollSpeed[1] = 0.01f;

    pixelShaderConstBufferData.time = 0.0f;
    pixelShaderConstBufferData.alpha = 0.8f;
}


// シェーダーの更新
void WaterShader::Update(void)
{
    // 時間を進める
    const float TIME_SPEED = (1.0f / 60.0f) * TimeScale::Get();

    vertexShaderConstBufferData.time += TIME_SPEED;
    pixelShaderConstBufferData.time += TIME_SPEED;
}


// シェーダーを描画に適用
void WaterShader::Apply(void)
{
    // 通常のShaderを適用
    ShaderBase::Apply();


    // 頂点シェーダー定数バッファ
    if (vertexShaderConstBufferHandle >= 0) {
        auto* buffer =
            static_cast<VertexShaderConstBuffer*>(
                GetBufferShaderConstantBuffer(
                    vertexShaderConstBufferHandle
                )
                );

        *buffer = vertexShaderConstBufferData;

        // CPU側 → GPU側へ反映
        UpdateShaderConstantBuffer(vertexShaderConstBufferHandle);

        // WaterVSの register(b4) に設定
        SetShaderConstantBuffer(vertexShaderConstBufferHandle, DX_SHADERTYPE_VERTEX, 4);
    }


    // ピクセルシェーダー定数バッファ
    if (pixelShaderConstBufferHandle >= 0) {
        auto* buffer =
            static_cast<PixelShaderConstBuffer*>(
                GetBufferShaderConstantBuffer(
                    pixelShaderConstBufferHandle
                )
                );

        *buffer = pixelShaderConstBufferData;

        // CPU側 → GPU側へ反映
        UpdateShaderConstantBuffer(pixelShaderConstBufferHandle);

        // WaterPSの register(b4) に設定
        SetShaderConstantBuffer(pixelShaderConstBufferHandle, DX_SHADERTYPE_PIXEL, 4);
    }
}


// シェーダーの設定リセット
void WaterShader::ResetApply(void)
{
    // 定数バッファを解除
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_VERTEX, 4);

    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 4);

    // Shader本体を解除
    ShaderBase::ResetApply();
}


// シェーダー固有リソースの解放
void WaterShader::Release(void)
{
    // 頂点シェーダー定数バッファ
    if (vertexShaderConstBufferHandle >= 0) {
        DeleteShaderConstantBuffer(vertexShaderConstBufferHandle);
        vertexShaderConstBufferHandle = -1;
    }


    // ピクセルシェーダー定数バッファ
    if (pixelShaderConstBufferHandle >= 0) {
        DeleteShaderConstantBuffer(pixelShaderConstBufferHandle);
        pixelShaderConstBufferHandle = -1;
    }

    // ShaderResourceManagerから借りているShaderハンドル自体は
    // WaterShaderではDeleteしない
    ShaderBase::Release();
}