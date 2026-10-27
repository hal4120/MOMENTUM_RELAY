#include "ModelDamageFlashShader.h"

#include <algorithm>
#include <cmath>

#include "../../../pch.h"

#include "../../../Manager/Shader/ShaderResourceManager.h"

#include "../../../Manager/TimeScale/TimeScale.h"

ModelDamageFlashShader::ModelDamageFlashShader(int duration, int count, float power, SHADER_VERTEX_TYPE type) :
    ShaderBase(),
    vertexType(type),
    durationFrames((std::max)(1, duration)),
    flashes((std::max)(1, count)),
    elapsedFrames(0),
    strength(std::clamp(power, 0.0f, 1.0f)),
    constantBufferHandle(-1)
{
}

void ModelDamageFlashShader::Init(void)
{
    // シェーダーハンドル管理クラスを取得
    ShaderResourceManager& manager = ShaderResourceManager::GetIns();

    // 使用する頂点シェーダーを頂点タイプを参照して決定する
    VERTEX_SHADER_TYPE useVertexShaderType = VERTEX_SHADER_TYPE::ModelDamageFlash1Frame;
    useVertexShaderType = (VERTEX_SHADER_TYPE)((int)useVertexShaderType + (int)vertexType);

    // 頂点シェーダーの生成
    manager.CreateVertexShader(useVertexShaderType);
    // ピクセルシェーダーの生成
    manager.CreatePixelShader(PIXEL_SHADER_TYPE::ModelDamageFlash);

    // 使用する頂点シェーダーを保持
    vertexShaderHandle = manager.GetVertexShader(useVertexShaderType);

    // 使用するピクセルシェーダーを保持
    pixelShaderHandle = manager.GetPixelShader(PIXEL_SHADER_TYPE::ModelDamageFlash);

    // 定数バッファの生成
    constantBufferHandle = CreateShaderConstantBuffer(sizeof(float) * 4);

}

void ModelDamageFlashShader::Update(void)
{
    if (IsEnd()) return;

    elapsedFrames += TimeScale::Get();
}

void ModelDamageFlashShader::Apply(void)
{
    float intensity = 0.0f;
    if (!IsEnd())
    {
        const float progress = static_cast<float>(elapsedFrames) / durationFrames;
        // 0 -> 1 -> 0 を flashes 回繰り返す
        const float wave = std::abs(std::sin(progress * flashes * DX_PI_F));
        // 後半に向けて少し減衰
        intensity = strength * wave * (1.0f - 0.35f * progress);
    }

    float* data = static_cast<float*>(GetBufferShaderConstantBuffer(constantBufferHandle));
    if (data)
    {
        data[0] = intensity;
        data[1] = data[2] = data[3] = 0.0f;
        UpdateShaderConstantBuffer(constantBufferHandle);
        SetShaderConstantBuffer(constantBufferHandle, DX_SHADERTYPE_PIXEL, FLASH_REGISTER);
    }
    ShaderBase::Apply();
}

void ModelDamageFlashShader::ResetApply(void)
{
    ShaderBase::ResetApply();
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, FLASH_REGISTER);
}

bool ModelDamageFlashShader::IsEnd(void) const
{
    return elapsedFrames >= durationFrames;
}

void ModelDamageFlashShader::Release(void)
{
    if (constantBufferHandle != -1) {
        DeleteShaderConstantBuffer(constantBufferHandle);

        constantBufferHandle = -1;
    }
    if (vertexShaderHandle != -1) vertexShaderHandle = -1;
    if (pixelShaderHandle != -1) pixelShaderHandle = -1;
    ShaderBase::Release();
}
