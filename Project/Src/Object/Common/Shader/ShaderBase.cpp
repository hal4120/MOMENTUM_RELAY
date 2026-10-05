#include "ShaderBase.h"

#include "../../../pch.h"

ShaderBase::ShaderBase() :
    vertexShaderHandle(-1),
    pixelShaderHandle(-1)
{
}

void ShaderBase::Apply(void)
{
    MV1SetUseOrigShader(true);

    // 頂点シェーダーの適用
    if (vertexShaderHandle != -1) { SetUseVertexShader(vertexShaderHandle); }

    // ピクセルシェーダーの適用
    if (pixelShaderHandle != -1) { SetUsePixelShader(pixelShaderHandle); }
}

void ShaderBase::ResetApply(void)
{
    // 頂点シェーダーの適用リセット
    SetUseVertexShader(-1);

    // ピクセルシェーダーの適用リセット
    SetUsePixelShader(-1);

    MV1SetUseOrigShader(false);
}

void ShaderBase::Release(void)
{
    // 頂点シェーダー
    vertexShaderHandle = -1;

    // ピクセルシェーダー
    pixelShaderHandle = -1;
}