#pragma once

#include "ShaderBase.h"

#include "../../../Manager/Shader/ShaderTypeDefine.h"

// ダメージ時にモデルを赤く点滅させる一時的なシェーダー
class ModelDamageFlashShader : public ShaderBase
{
public:
    explicit ModelDamageFlashShader(
        int durationFrames = 24,
        int flashes = 3,
        float strength = 0.85f,
        SHADER_VERTEX_TYPE type = SHADER_VERTEX_TYPE::Frame8
    );
    ~ModelDamageFlashShader() override = default;

    void Init(void) override;
    void Update(void) override;
    void Apply(void) override;
    void ResetApply(void) override;
    bool IsEnd(void) const override;
    void Release(void) override;

private:

    static constexpr int FLASH_REGISTER = 4;

    SHADER_VERTEX_TYPE vertexType;
    int durationFrames;
    int flashes;
    float elapsedFrames;
    float strength;
    int constantBufferHandle;
};
