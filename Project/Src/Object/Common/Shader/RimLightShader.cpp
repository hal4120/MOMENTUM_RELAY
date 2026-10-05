#include "RimLightShader.h"

#include "../../../pch.h"

#include "../../../Manager/Shader/ShaderResourceManager.h"

void RimLightShader::Init(void)
{
    // シェーダーハンドル管理クラスを取得
    ShaderResourceManager& manager = ShaderResourceManager::GetIns();

    // 使用する頂点シェーダーを頂点タイプを参照して決定する
    VERTEX_SHADER_TYPE useVertexShaderType = VERTEX_SHADER_TYPE::RimLight1Frame;
    useVertexShaderType = (VERTEX_SHADER_TYPE)((int)useVertexShaderType + (int)vertexType);

    // 頂点シェーダーの生成
    manager.CreateVertexShader(useVertexShaderType);
    // ピクセルシェーダーの生成
    manager.CreatePixelShader(PIXEL_SHADER_TYPE::RimLight);

    // 使用する頂点シェーダーを保持
    vertexShaderHandle = manager.GetVertexShader(useVertexShaderType);

    // 使用するピクセルシェーダーを保持
    pixelShaderHandle = manager.GetPixelShader(PIXEL_SHADER_TYPE::RimLight);
}