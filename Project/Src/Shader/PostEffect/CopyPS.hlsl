#include "PostEffectHeader.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    return GetSceneColor(input);
}