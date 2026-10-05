#include "VertexInputType.hlsli"
#if (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_1FRAME)
struct VertexInput { float3 pos:POSITION; float3 norm:NORMAL; float4 diffuse:COLOR0; float4 specular:COLOR1; float4 uv0:TEXCOORD0; float4 uv1:TEXCOORD1; };
#elif (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_4FRAME)
struct VertexInput { float3 pos:POSITION; float3 norm:NORMAL; float4 diffuse:COLOR0; float4 specular:COLOR1; float4 uv0:TEXCOORD0; float4 uv1:TEXCOORD1; int4 blendIndices0:BLENDINDICES0; float4 blendWeight0:BLENDWEIGHT0; };
#elif (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_8FRAME)
struct VertexInput { float3 pos:POSITION; float3 norm:NORMAL; float4 diffuse:COLOR0; float4 specular:COLOR1; float4 uv0:TEXCOORD0; float4 uv1:TEXCOORD1; int4 blendIndices0:BLENDINDICES0; float4 blendWeight0:BLENDWEIGHT0; int4 blendIndices1:BLENDINDICES1; float4 blendWeight1:BLENDWEIGHT1; };
#elif (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_NMAP_1FRAME)
struct VertexInput { float3 pos:POSITION; float3 norm:NORMAL; float4 diffuse:COLOR0; float4 specular:COLOR1; float4 uv0:TEXCOORD0; float4 uv1:TEXCOORD1; float3 tan:TANGENT0; float3 bin:BINORMAL0; };
#elif (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_NMAP_4FRAME)
struct VertexInput { float3 pos:POSITION; float3 norm:NORMAL; float4 diffuse:COLOR0; float4 specular:COLOR1; float4 uv0:TEXCOORD0; float4 uv1:TEXCOORD1; float3 tan:TANGENT0; float3 bin:BINORMAL0; int4 blendIndices0:BLENDINDICES0; float4 blendWeight0:BLENDWEIGHT0; };
#elif (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_NMAP_8FRAME)
struct VertexInput { float3 pos:POSITION; float3 norm:NORMAL; float4 diffuse:COLOR0; float4 specular:COLOR1; float4 uv0:TEXCOORD0; float4 uv1:TEXCOORD1; float3 tan:TANGENT0; float3 bin:BINORMAL0; int4 blendIndices0:BLENDINDICES0; float4 blendWeight0:BLENDWEIGHT0; int4 blendIndices1:BLENDINDICES1; float4 blendWeight1:BLENDWEIGHT1; };
#elif (VERTEX_INPUT == DX_VERTEX3DSHADER)
struct VertexInput { float3 pos:POSITION0; float4 SubPosition:POSITION1; float3 Normal:NORMAL; float3 Tangent:TANGENT; float3 Binormal:BINORMAL; float4 diffuse:COLOR0; float4 Specular:COLOR1; float2 TexCoords0:TEXCOORD0; float2 TexCoords1:TEXCOORD1; };
#endif
