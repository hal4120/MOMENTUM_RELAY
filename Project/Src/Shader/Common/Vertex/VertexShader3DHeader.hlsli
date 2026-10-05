#if !defined(VERTEX_INPUT)
#include "VertexInputType.hlsli"
#define VERTEX_INPUT DX_MV1_VERTEX_TYPE_1FRAME
#endif
#include "VertexInput.hlsli"
#include "CommonShader3DHeader.hlsli"
#if !defined(VS_INPUT)
#define VS_INPUT VertexInput
#endif
#if !defined(VS_OUTPUT)
#define VS_OUTPUT VertexToPixel
#endif
#define DX_D3D11_VS_CONST_TEXTURE_MATRIX_NUM (3)
#define DX_D3D11_VS_CONST_WORLD_MAT_NUM (54)
struct VsBase { matrix antiViewportMatrix; matrix projectionMatrix; float4x3 viewMatrix; float4x3 localWorldMatrix; float4 toonOutLineSize; float diffuseSource; float specularSource; float mulSpecularColor; float padding; };
struct VsOtherMatrix { float4 shadowMapLightViewProjectionMatrix[3][4]; float4 textureMatrix[DX_D3D11_VS_CONST_TEXTURE_MATRIX_NUM][2]; };
struct VsLocalWorldMatrix { float4 lwMatrix[DX_D3D11_VS_CONST_WORLD_MAT_NUM*3]; };
cbuffer cbD3D11_CONST_BUFFER_COMMON:register(b0){ Common g_common; };
cbuffer cbD3D11_CONST_BUFFER_VS_BASE:register(b1){ VsBase g_base; };
cbuffer cbD3D11_CONST_BUFFER_VS_OTHERMATRIX:register(b2){ VsOtherMatrix g_otherMatrix; };
cbuffer cbD3D11_CONST_BUFFER_VS_LOCALWORLDMATRIX:register(b3){ VsLocalWorldMatrix g_localWorldMatrix; };
