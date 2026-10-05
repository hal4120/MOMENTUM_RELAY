#include "ShadowVSOutput.hlsli"
#define VS_OUTPUT ShadowVertexToPixel
#include "../Common/Vertex/VertexShader3DHeader.hlsli"
#include "../Common/Vertex/ModelVertexTransform.hlsli"

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT o = (VS_OUTPUT)0;
    float3 worldPos = ModelToWorldPosition(input);
    float3 worldNormal = ModelToWorldNormal(input);
    float3 viewPos = WorldToViewPosition(worldPos);

    o.svPos = ViewToProjection(viewPos);
    o.vwPos = viewPos;
    o.normal = WorldToViewNormal(worldNormal);
    o.uv = input.uv0.xy;
    o.diffuse = input.diffuse;

    float4 wp = float4(worldPos, 1.0f);
    o.shadowPos.x = dot(wp, g_otherMatrix.shadowMapLightViewProjectionMatrix[0][0]);
    o.shadowPos.y = dot(wp, g_otherMatrix.shadowMapLightViewProjectionMatrix[0][1]);
    o.shadowPos.z = dot(wp, g_otherMatrix.shadowMapLightViewProjectionMatrix[0][2]);
    o.shadowPos.w = dot(wp, g_otherMatrix.shadowMapLightViewProjectionMatrix[0][3]);

    return o;
}
