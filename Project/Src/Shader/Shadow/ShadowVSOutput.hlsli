struct ShadowVertexToPixel
{
    float4 svPos       : SV_POSITION;
    float2 uv          : TEXCOORD0;
    float3 vwPos       : TEXCOORD1;
    float3 normal      : TEXCOORD2;
    float4 diffuse     : COLOR0;
    float4 shadowPos   : TEXCOORD3;
};
