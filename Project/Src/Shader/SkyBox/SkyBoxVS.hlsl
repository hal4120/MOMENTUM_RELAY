#include "../Common/Vertex/VertexInputType.hlsli"

#define VERTEX_INPUT DX_VERTEX3DSHADER

#include "../Common/Vertex/VertexShader3DHeader.hlsli"

struct SkyBoxOutput
{
    float4 svPos : SV_POSITION;
    float3 direction : TEXCOORD0;
};

SkyBoxOutput main(VS_INPUT input)
{
    SkyBoxOutput output = (SkyBoxOutput) 0;

    // 立方体中心からの方向
    output.direction = input.pos;

    // カメラの回転のみ適用する
    float3 viewPos = mul(
        float4(input.pos, 0.0f),
        g_base.viewMatrix
    );

    // 射影変換
    output.svPos = mul(
        float4(viewPos, 1.0f),
        g_base.projectionMatrix
    );

    // 常に最奥に配置
    output.svPos.z = output.svPos.w;

    return output;
}