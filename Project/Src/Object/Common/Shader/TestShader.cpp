#include "TestShader.h"

#include "../../../pch.h"

#include "../../../Manager/Shader/ShaderResourceManager.h"

TestShader::TestShader() :
    constantBufferHandle(-1),
    param{}
{
}

void TestShader::Init(void)
{
    ShaderResourceManager& manager = ShaderResourceManager::GetIns();

    // 頂点シェーダーは使用しない
    vertexShaderHandle = -1;

    //manager.CreatePixelShader(PIXEL_SHADER_TYPE::Test);
    //manager.CreatePixelShader(PIXEL_SHADER_TYPE::Test);

    //pixelShaderHandle = manager.GetPixelShader(PIXEL_SHADER_TYPE::Test);

    param.time = 0.0f;
    param.scale = 20.0f;
    param.intensity = 1.0f;

    constantBufferHandle = CreateShaderConstantBuffer(sizeof(PARAM));
}

void TestShader::Update(void)
{
    param.time += 1.0f / 60.0f;
}

//void TestShader::Apply(void)
//{
//    PARAM* buffer = static_cast<PARAM*>(GetBufferShaderConstantBuffer(constantBufferHandle));
//
//    *buffer = param;
//
//    UpdateShaderConstantBuffer(constantBufferHandle);
//
//    SetShaderConstantBuffer(constantBufferHandle, DX_SHADERTYPE_PIXEL, 10);
//
//    ShaderBase::Apply();
//}
//
//void TestShader::ResetApply(void)
//{
//    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 10);
//
//    ShaderBase::ResetApply();
//}

void TestShader::Apply(void)
{
    ShaderBase::Apply();
}

void TestShader::ResetApply(void)
{
    ShaderBase::ResetApply();
}

void TestShader::Release(void)
{
    if (constantBufferHandle != -1) {
        DeleteShaderConstantBuffer(constantBufferHandle);

        constantBufferHandle = -1;
    }

    ShaderBase::Release();
}