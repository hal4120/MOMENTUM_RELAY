#include "../Vertex/CommonShader3DHeader.hlsli"
#if !defined(PS_INPUT)
#define PS_INPUT VertexToPixel
#endif
#if !defined(PS_OUTPUT)
#define PS_OUTPUT float4
#endif
struct ShadowMap { float adjustDepth; float gradationParam; float enable_Light0; float enable_Light1; float enable_Light2; float3 padding; };
struct PsBase { float4 factorColor; float mulAlphaColor; float alphaTestRef; float2 padding1; int alphaTestCmpMode; int noLightAngleAttenuation; int2 padding2; float4 ignoreTextureColor; float4 drawAddColor; };
struct PsShadowMap { ShadowMap data[3]; };
cbuffer cbD3D11_CONST_BUFFER_COMMON:register(b0){ Common g_common; };
cbuffer cbD3D11_CONST_BUFFER_PS_BASE:register(b1){ PsBase g_base; };
cbuffer cbD3D11_CONST_BUFFER_PS_SHADOWMAP:register(b2){ PsShadowMap g_shadowMap; };
SamplerState diffuseMapSampler:register(s0);
Texture2D diffuseMapTexture:register(t0);
