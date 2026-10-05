#include "../Common/VertexToPixelHeader.hlsli"
#define VS_OUTPUT VertexToPixelLit
#include "../Common/Vertex/VertexShader3DHeader.hlsli"
#include "../Common/Vertex/ModelVertexTransform.hlsli"
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT o=(VS_OUTPUT)0;
    float3 wp=ModelToWorldPosition(input), wn=ModelToWorldNormal(input);
    float3 vp=WorldToViewPosition(wp), vn=WorldToViewNormal(wn);
    o.svPos=ViewToProjection(vp); o.vwPos=vp; o.normal=vn; o.uv=input.uv0.xy; o.diffuse=input.diffuse;
    float3 toCamera=normalize(-vp);
    float rim=1.0-saturate(abs(dot(normalize(vn),toCamera)));
    rim=pow(rim,2.0);
    float3 rimColor=float3(0,1,0);
    o.effectColor=float4(rimColor*rim,1);
    return o;
}
