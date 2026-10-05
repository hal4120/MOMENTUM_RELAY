#include "PostEffectHeader.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color = GetSceneColor(input);

    float gray = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));

    return float4(gray, gray, gray, color.a);
}