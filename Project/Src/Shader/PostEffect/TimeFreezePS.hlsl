#include "PostEffectHeader.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.uv;
    uint w, h;
    sceneTexture.GetDimensions(w, h);
    float aspect = (float)w / max((float)h, 1.0f);
    float2 centered = (uv - 0.5f) * float2(aspect, 1.0f);
    float radius = length(centered);

    // Slight edge-only chromatic separation; keep center crisp for aiming.
    float edge = smoothstep(0.25f, 0.95f, radius);
    float2 shift = float2(1.5f / max((float)w, 1.0f), 0.0f) * edge;
    float4 src = GetSceneColor(uv);
    float3 rgb = float3(
        GetSceneColor(saturate(uv + shift)).r,
        src.g,
        GetSceneColor(saturate(uv - shift)).b
    );

    float gray = dot(rgb, float3(0.299f, 0.587f, 0.114f));
    rgb = lerp(rgb, gray.xxx, 0.48f);
    rgb *= float3(0.78f, 0.90f, 1.12f);
    rgb += float3(0.025f, 0.035f, 0.06f);
    rgb *= 1.0f - 0.28f * smoothstep(0.30f, 0.95f, radius);
    return float4(saturate(rgb), src.a);
}
