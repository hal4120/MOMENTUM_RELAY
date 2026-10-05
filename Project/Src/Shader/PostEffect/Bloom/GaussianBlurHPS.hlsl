#include "../PostEffectHeader.hlsli"

cbuffer BlurParameter : register(b4)
{
    float2 texelSize;
    float2 padding;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    static const float w0 = 0.2270270270f;
    static const float w1 = 0.3162162162f;
    static const float w2 = 0.0702702703f;

    float2 o1 = float2(texelSize.x * 1.3846153846f, 0.0f);
    float2 o2 = float2(texelSize.x * 3.2307692308f, 0.0f);

    float4 color = sceneTexture.Sample(sceneSampler, input.uv) * w0;
    color += sceneTexture.Sample(sceneSampler, input.uv + o1) * w1;
    color += sceneTexture.Sample(sceneSampler, input.uv - o1) * w1;
    color += sceneTexture.Sample(sceneSampler, input.uv + o2) * w2;
    color += sceneTexture.Sample(sceneSampler, input.uv - o2) * w2;
    return color;
}
