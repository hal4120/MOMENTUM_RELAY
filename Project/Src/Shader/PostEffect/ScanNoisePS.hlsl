#include "PostEffectHeader.hlsli"

cbuffer ScanNoiseParam : register(b0)
{
    float time;
    float strength;
    float speed;
    float scale;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.uv;
    float y = floor(input.pos.y / max(scale, 1.0f));
    float noise = Rand(float2(y, floor(time * speed)));
    float2 shiftedUV = saturate(uv + float2((noise - 0.5f) * strength, 0.0f));
    return lerp(GetSceneColor(uv), GetSceneColor(shiftedUV), saturate(strength * 8.0f));
}
