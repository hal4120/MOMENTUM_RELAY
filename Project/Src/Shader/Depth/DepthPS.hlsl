#include "../Common/VertexToPixelHeader.hlsli"
#define PS_INPUT VertexToPixelLit
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

cbuffer DepthParameter : register(b4)
{
    float nearPlane;
    float farPlane;
    float2 padding;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float distanceFromCamera = length(input.vwPos);
    float depth = saturate((distanceFromCamera - nearPlane) / (farPlane - nearPlane));
    return float4(depth, depth, depth, 1.0f);
}
