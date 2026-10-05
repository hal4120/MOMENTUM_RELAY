#include "../Common/VertexToPixelHeader.hlsli"
#define PS_INPUT VertexToPixelLit
#include "../Common/Pixel/PixelShader3DHeader.hlsli"
#include "../Common/Pixel/ModelPixelCommon.hlsli"
float4 main(PS_INPUT i):SV_TARGET
{
    float4 tex=diffuseMapTexture.Sample(diffuseMapSampler,i.uv);
    float4 base=tex*i.diffuse;
    float3 color=CalcModelColor(base.rgb,i.vwPos,i.normal);
    color=FinalizeModelColor(color,i.vwPos);
    return float4(color,base.a);
}
