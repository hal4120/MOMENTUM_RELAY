#include "PostEffectHeader.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 src = GetSceneColor(input);
    uint w, h;
    sceneTexture.GetDimensions(w, h);
    float2 pixel = (input.uv - 0.5f) * float2((float)w, (float)h);

    // Solid white cross: 2px thick, 18px across at any resolution.
    float halfThickness = 1.0f;
    float halfLength = 9.0f;
    bool horizontal = abs(pixel.x) <= halfLength && abs(pixel.y) <= halfThickness;
    bool vertical   = abs(pixel.y) <= halfLength && abs(pixel.x) <= halfThickness;
    if (horizontal || vertical)
        return float4(1.0f, 1.0f, 1.0f, src.a);
    return src;
}
