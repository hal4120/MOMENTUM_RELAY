#include "../Common/VertexToPixelHeader.hlsli"
#define VS_OUTPUT VertexToPixelLit
#include "../Common/Vertex/VertexShader3DHeader.hlsli"
#include "../Common/Vertex/ModelVertexTransform.hlsli"

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT o = (VS_OUTPUT)0;
    float3 worldPos = ModelToWorldPosition(input);
    float3 viewPos = WorldToViewPosition(worldPos);

    o.svPos = ViewToProjection(viewPos);
    o.vwPos = viewPos;
    o.uv = input.uv0.xy;
    o.diffuse = input.diffuse;
    return o;
}
