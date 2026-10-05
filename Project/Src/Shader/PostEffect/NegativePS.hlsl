#include "PostEffectHeader.hlsli"

cbuffer NegativeParam : register(b0)
{
    float intensity;
    float3 padding;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 src = GetSceneColor(input);
    return float4(lerp(src.rgb, 1.0f - src.rgb, saturate(intensity)), src.a);
}
