#include "ShadowVSOutput.hlsli"
#define PS_INPUT ShadowVertexToPixel
#define SHADOWMAP 1
#include "../Common/Pixel/PixelShader3DHeader.hlsli"
#include "../Common/Pixel/ModelPixelCommon.hlsli"

SamplerState shadowMap0Sampler : register(s8);
Texture2D shadowMap0Texture : register(t8);

float CalcShadow(float4 shadowPos)
{
    float shadow = 1.0f;

    float3 p = float3(0.0f, 0.0f, 0.0f);
    float2 uv = float2(0.0f, 0.0f);

    float mapDepth = 1.0f;
    float currentDepth = 0.0f;

    const float bias = 0.0015f;

    if (abs(shadowPos.w) >= 0.000001f)
    {
        p = shadowPos.xyz / shadowPos.w;

        uv.x = p.x * 0.5f + 0.5f;
        uv.y = -p.y * 0.5f + 0.5f;

        if (uv.x >= 0.0f && uv.x <= 1.0f &&
            uv.y >= 0.0f && uv.y <= 1.0f)
        {
            mapDepth =
                shadowMap0Texture.Sample(
                    shadowMap0Sampler,
                    uv
                ).r;

            currentDepth = p.z;

            if (currentDepth - bias > mapDepth)
            {
                shadow = 0.35f;
            }
        }
    }

    return shadow;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 tex = diffuseMapTexture.Sample(diffuseMapSampler, input.uv);
    float4 base = tex * input.diffuse;

    ModelLightingResult lighting = CalcModelLighting(input.vwPos, input.normal);
    float shadow = CalcShadow(input.shadowPos);

    float3 color =
        base.rgb * (lighting.ambient + lighting.diffuse * shadow)
        + lighting.specular * shadow;

    color = FinalizeModelColor(color, input.vwPos);
    return float4(color, base.a);
}
