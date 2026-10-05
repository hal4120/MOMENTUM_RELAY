#include "../PostEffectHeader.hlsli"

SamplerState bloomSampler : register(s1);
Texture2D bloomTexture : register(t1);

cbuffer CompositeParameter : register(b4)
{
    float bloomStrength;
    float3 padding;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 scene = sceneTexture.Sample(sceneSampler, input.uv);
    float3 bloom = bloomTexture.Sample(bloomSampler, input.uv).rgb;
    return float4(scene.rgb + bloom * bloomStrength, scene.a);
}
