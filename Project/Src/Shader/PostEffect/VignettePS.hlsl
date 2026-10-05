#include "PostEffectHeader.hlsli"

cbuffer VignetteParam : register(b0)
{
    float intensity;
    float radius;
    float softness;
    float padding;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    uint textureWidth, textureHeight;
    sceneTexture.GetDimensions(textureWidth, textureHeight);
    float2 screenSize = float2(textureWidth, textureHeight);
    float2 uv = input.uv;
    float4 color = GetSceneColor(uv);
    float2 p = uv - 0.5f;
    p.x *= screenSize.x / screenSize.y;
    float mask = 1.0f - smoothstep(radius, radius + max(softness, 0.0001f), length(p));
    color.rgb *= lerp(1.0f, mask, saturate(intensity));
    return color;
}
