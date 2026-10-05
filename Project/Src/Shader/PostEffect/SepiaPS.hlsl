#include "PostEffectHeader.hlsli"

cbuffer SepiaParam : register(b0)
{
    float intensity;
    float3 padding;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 src = GetSceneColor(input);
    float3 sepia;
    sepia.r = dot(src.rgb, float3(0.393f, 0.769f, 0.189f));
    sepia.g = dot(src.rgb, float3(0.349f, 0.686f, 0.168f));
    sepia.b = dot(src.rgb, float3(0.272f, 0.534f, 0.131f));
    return float4(lerp(src.rgb, saturate(sepia), saturate(intensity)), src.a);
}
