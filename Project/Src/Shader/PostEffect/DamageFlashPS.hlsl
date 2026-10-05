#include "PostEffectHeader.hlsli"

cbuffer DamageFlashParam : register(b0)
{
    float intensity;
    float edgeStrength;
    float2 padding;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    uint textureWidth, textureHeight;
    sceneTexture.GetDimensions(textureWidth, textureHeight);
    float2 screenSize = float2(textureWidth, textureHeight);
    float2 uv = input.uv;
    float4 src = GetSceneColor(uv);
    float2 p = uv - 0.5f;
    p.x *= screenSize.x / screenSize.y;
    float edge = smoothstep(0.15f, 0.85f, length(p));
    float amount = saturate(intensity) * lerp(1.0f, edge, saturate(edgeStrength));
    float3 result = src.rgb + float3(0.65f, -0.15f, -0.15f) * amount;
    return float4(saturate(result), src.a);
}
