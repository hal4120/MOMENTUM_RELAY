#include "PostEffectHeader.hlsli"

cbuffer OldFilmParam : register(b0)
{
    float time;
    float grainStrength;
    float scratchStrength;
    float intensity;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.uv;
    float4 src = GetSceneColor(uv);
    float gray = dot(src.rgb, float3(0.299f, 0.587f, 0.114f));
    float3 sepia = float3(gray * 1.15f, gray, gray * 0.78f);
    float grain = Rand(floor(input.pos.xy) + floor(time * 60.0f)) - 0.5f;
    float scratchSeed = Rand(float2(floor(input.pos.x), floor(time * 4.0f)));
    float scratch = step(0.985f, scratchSeed) * scratchStrength;
    float3 film = sepia + grain * grainStrength + scratch;
    return float4(lerp(src.rgb, saturate(film), saturate(intensity)), src.a);
}
