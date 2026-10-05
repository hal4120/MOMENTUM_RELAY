#include "../PostEffectHeader.hlsli"

cbuffer BloomParameter : register(b4)
{
    float threshold;
    float knee;
    float intensity;
    float padding;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color = sceneTexture.Sample(sceneSampler, input.uv);

    float brightness = max(color.r, max(color.g, color.b));
    float soft = saturate((brightness - threshold + knee) / max(knee * 2.0f, 0.0001f));
    soft = soft * soft * (3.0f - 2.0f * soft);

    float contribution = max(brightness - threshold, 0.0f) + soft * knee;
    contribution /= max(brightness, 0.0001f);

    return float4(color.rgb * contribution * intensity, color.a);
}
