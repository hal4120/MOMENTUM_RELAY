#include "../Common/VertexToPixelHeader.hlsli"
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

cbuffer WaterPSParameter : register(b4)
{
    float2 g_uvScrollSpeed;
    float g_time;
    float g_alpha;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // 教材通り、UVスクロールはPS側。
    float2 uv = input.uv + g_uvScrollSpeed * g_time;
    float4 color = diffuseMapTexture.Sample(diffuseMapSampler, uv) * input.diffuse;
    color.a *= g_alpha;
    return color;
}
