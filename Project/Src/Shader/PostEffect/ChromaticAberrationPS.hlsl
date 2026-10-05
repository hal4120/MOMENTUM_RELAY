#include "PostEffectHeader.hlsli"

cbuffer ChromaticAberrationParam : register(b0)
{
    float strength;
    float radial;
    float2 padding;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.uv;
    float2 p = uv - 0.5f;
    float2 dir = normalize(p + 0.000001f);
    float amount = strength * lerp(1.0f, length(p) * 2.0f, saturate(radial));
    float2 offset = dir * amount;

    float r = sceneTexture.Sample(sceneSampler, saturate(uv + offset)).r;
    float g = sceneTexture.Sample(sceneSampler, uv).g;
    float b = sceneTexture.Sample(sceneSampler, saturate(uv - offset)).b;
    return float4(r, g, b, 1.0f);
}
