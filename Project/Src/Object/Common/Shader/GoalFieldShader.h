#pragma once
#include "ShaderBase.h"
#include "../../../Manager/Shader/ShaderTypeDefine.h"

class GoalFieldShader : public ShaderBase
{
public:
    explicit GoalFieldShader(SHADER_VERTEX_TYPE type = SHADER_VERTEX_TYPE::Frame8);
    void Init(void) override;
    void Update(void) override;
    void Apply(void) override;
    void ResetApply(void) override;
    void Release(void) override;
    void SetSpeed(float value) { speed = value; }
    void SetBrightness(float value) { brightness = value; }
    void SetOpacity(float value) { opacity = value; }
private:
    SHADER_VERTEX_TYPE vertexType;
    int constantBufferHandle;
    float elapsed;
    float speed;
    float brightness;
    float opacity;
    static constexpr int BUFFER_REGISTER = 4;
};
