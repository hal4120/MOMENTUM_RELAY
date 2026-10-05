#include "PostEffectHeader.hlsli"

cbuffer CRTParam : register(b0)
{
    float time;
    float intensity;
    float distortion;
    float scanLineStrength;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.uv;
    float2 p = uv - 0.5f;
    float dist2 = dot(p, p);
    float2 distortedUV = uv + p * dist2 * distortion;

    if (distortedUV.x < 0.0f || distortedUV.x > 1.0f ||
        distortedUV.y < 0.0f || distortedUV.y > 1.0f)
    {
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    const float chromaOffset = 0.0015f;
    float r = sceneTexture.Sample(sceneSampler, distortedUV + float2(chromaOffset, 0.0f)).r;
    float g = sceneTexture.Sample(sceneSampler, distortedUV).g;
    float b = sceneTexture.Sample(sceneSampler, distortedUV - float2(chromaOffset, 0.0f)).b;
    float3 color = float3(r, g, b);

    float scanLine = sin(input.pos.y * 3.14159265f) * 0.5f + 0.5f;
    color *= lerp(1.0f, 1.0f - scanLineStrength, scanLine);
    color *= 1.0f + sin(time * 60.0f) * 0.015f;

    float vignette = 1.0f - smoothstep(0.25f, 0.75f, length(p));
    color *= lerp(1.0f, vignette, 0.45f);

    float4 original = GetSceneColor(uv);
    return float4(lerp(original.rgb, color, saturate(intensity)), original.a);
}
