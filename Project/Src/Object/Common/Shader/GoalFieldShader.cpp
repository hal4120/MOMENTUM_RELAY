#include "GoalFieldShader.h"
#include "../../../pch.h"
#include "../../../Manager/Shader/ShaderResourceManager.h"

GoalFieldShader::GoalFieldShader(SHADER_VERTEX_TYPE type) :
    ShaderBase(),
    vertexType(type),
    constantBufferHandle(-1),

    elapsed(0.0f),
    speed(1.0f),
    brightness(2.0f),
    opacity(0.3f),
    waveHeight(0.08f),
    waveFrequency(3.0f),
    radius(1.0f),
    height(2.0f)
{
}

void GoalFieldShader::Init(void)
{
    auto& manager = ShaderResourceManager::GetIns();
    auto vs = static_cast<VERTEX_SHADER_TYPE>(static_cast<int>(VERTEX_SHADER_TYPE::GoalField1Frame) + static_cast<int>(vertexType));
    manager.CreateVertexShader(vs);
    manager.CreatePixelShader(PIXEL_SHADER_TYPE::GoalField);
    vertexShaderHandle = manager.GetVertexShader(vs);
    pixelShaderHandle = manager.GetPixelShader(PIXEL_SHADER_TYPE::GoalField);
    if (constantBufferHandle == -1) constantBufferHandle = CreateShaderConstantBuffer(sizeof(float) * 8);
}
void GoalFieldShader::Update(void) { elapsed += (1.0f / 60.0f); }
void GoalFieldShader::Apply(void)
{
    if (constantBufferHandle != -1) {
        float* p = static_cast<float*>(GetBufferShaderConstantBuffer(constantBufferHandle));
        if (p) {
            p[0] = elapsed; p[1] = speed; p[2] = brightness; p[3] = opacity;
            p[4] = waveHeight; p[5] = waveFrequency; p[6] = radius; p[7] = height;
            UpdateShaderConstantBuffer(constantBufferHandle);
            SetShaderConstantBuffer(constantBufferHandle, DX_SHADERTYPE_PIXEL, BUFFER_REGISTER);
            SetShaderConstantBuffer(constantBufferHandle, DX_SHADERTYPE_VERTEX, BUFFER_REGISTER);
        }
    }
    ShaderBase::Apply();
}
void GoalFieldShader::ResetApply(void)
{
    ShaderBase::ResetApply();
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, BUFFER_REGISTER);
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_VERTEX, BUFFER_REGISTER);
}
void GoalFieldShader::Release(void)
{
    if (constantBufferHandle != -1) { DeleteShaderConstantBuffer(constantBufferHandle); constantBufferHandle = -1; }
    ShaderBase::Release();
}
