#include "../Common/VertexToPixelHeader.hlsli"
#define PS_INPUT VertexToPixelLit
#include "../Common/Pixel/PixelShader3DHeader.hlsli"
#include "../Common/Pixel/ModelPixelCommon.hlsli"

cbuffer DamageFlashParams : register(b4)
{
    float g_flashIntensity;
    float3 g_padding;
};

float4 main(PS_INPUT i) : SV_TARGET
{
    float4 tex = diffuseMapTexture.Sample(diffuseMapSampler, i.uv);
    float4 base = tex * i.diffuse;
    float3 color = CalcModelColor(base.rgb, i.vwPos, i.normal);
    color = FinalizeModelColor(color, i.vwPos);

    // 元の陰影を少し残しつつ赤色へ寄せる
    float3 damageRed = float3(1.0f, 0.055f, 0.035f);
    color = lerp(color, damageRed, saturate(g_flashIntensity));
    return float4(color, base.a);
}
