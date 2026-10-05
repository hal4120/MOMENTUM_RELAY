struct VertexToPixel { float4 svPos:SV_POSITION; float4 diffuse:COLOR0; float4 specular:COLOR1; float2 uv:TEXCOORD0; };
struct VertexToPixelLit
{
    float4 svPos:SV_POSITION;
    float2 uv:TEXCOORD0;
    float3 vwPos:TEXCOORD1;
    float3 normal:TEXCOORD2;
    float4 diffuse:COLOR0;
    float3 lightDir:TEXCOORD3;
    float3 lightAtPos:TEXCOORD4;
    float4 effectColor:TEXCOORD5;
};
struct VertexToPixelShadow { float4 svPos:SV_POSITION; float2 uv:TEXCOORD0; float4 vwPos:TEXCOORD1; };
