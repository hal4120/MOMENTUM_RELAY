#include "PostEffectHeader.hlsli"

cbuffer WaveParam : register(b0)
{
    float time;
    float amplitude;
    float frequency;
    float speed;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.uv;
    uv.x += sin(uv.y * frequency + time * speed) * amplitude;
    return GetSceneColor(saturate(uv));
}
