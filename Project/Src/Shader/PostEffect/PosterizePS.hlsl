#include "PostEffectHeader.hlsli"

cbuffer PosterizeParam : register(b0)
{
    float levels;
    float intensity;
    float2 padding;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 src = GetSceneColor(input);
    float lv = max(levels, 2.0f);
    float3 poster = floor(src.rgb * (lv - 1.0f) + 0.5f) / (lv - 1.0f);
    return float4(lerp(src.rgb, poster, saturate(intensity)), src.a);
}
