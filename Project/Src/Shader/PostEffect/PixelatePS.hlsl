#include "PostEffectHeader.hlsli"

cbuffer PixelateParam : register(b0)
{
    float pixelSize;
    float intensity;
    float2 padding;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    uint textureWidth, textureHeight;
    sceneTexture.GetDimensions(textureWidth, textureHeight);
    float2 screenSize = float2(textureWidth, textureHeight);
    float2 uv = input.uv;
    float px = max(pixelSize, 1.0f);
    float2 pixelCount = screenSize / px;
    float2 pixelUV = (floor(uv * pixelCount) + 0.5f) / pixelCount;
    return lerp(GetSceneColor(uv), GetSceneColor(pixelUV), saturate(intensity));
}
