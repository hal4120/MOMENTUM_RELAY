#include "../Common/VertexToPixelHeader.hlsli"
#include "../Common/Vertex/VertexInputType.hlsli"
#define VERTEX_INPUT DX_MV1_VERTEX_TYPE_1FRAME
#include "../Common/Vertex/VertexShader3DHeader.hlsli"

// 自作VS定数バッファ用。
// C++側から SetShaderConstantBuffer で空いているユーザー用スロットへ設定すること。
cbuffer WaterVSParameter : register(b4)
{
    float2 g_uvScale;
    float g_time;
    float g_waveHeight;

    float g_waveLength;
    float g_waveSpeed;
    float2 g_waterPadding;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    float4 localPos = float4(input.pos, 1.0f);
    float3 worldPos = mul(localPos, g_base.localWorldMatrix);

    // 教材の「ワールド空間Xに応じてsinでYを動かす」。
    worldPos.y += sin(worldPos.x * g_waveLength + g_time * g_waveSpeed) * g_waveHeight;

    float3 viewPos = mul(float4(worldPos, 1.0f), g_base.viewMatrix);
    output.svPos = mul(float4(viewPos, 1.0f), g_base.projectionMatrix);

    // UVスケールはVS側。
    output.uv = input.uv0.xy * g_uvScale;
    output.diffuse = input.diffuse;
    output.specular = input.specular;

    return output;
}
