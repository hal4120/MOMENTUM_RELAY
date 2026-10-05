#ifndef MODEL_VERTEX_TRANSFORM_HLSLI
#define MODEL_VERTEX_TRANSFORM_HLSLI

// 教材の VertexInput / VertexShader3DHeader を前提にした共通変換処理。
// 1FRAME / 4FRAME / 8FRAME と、それぞれの NMAP 版に対応する。

void BuildModelWorldMatrix(VS_INPUT input, out float4 rows[3])
{
#if (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_1FRAME) || \
    (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_NMAP_1FRAME)

    // g_base.localWorldMatrix は float4x3。
    // mul(float4, float4x3) を使うため、1FRAMEではこの関数の rows は使用しない。
    rows[0] = 0.0f;
    rows[1] = 0.0f;
    rows[2] = 0.0f;

#elif (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_4FRAME) || \
      (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_NMAP_4FRAME)

    int4 bone = input.blendIndices0;
    float4 weight = input.blendWeight0;

    rows[0] = g_localWorldMatrix.lwMatrix[bone.x + 0] * weight.xxxx;
    rows[1] = g_localWorldMatrix.lwMatrix[bone.x + 1] * weight.xxxx;
    rows[2] = g_localWorldMatrix.lwMatrix[bone.x + 2] * weight.xxxx;

    rows[0] += g_localWorldMatrix.lwMatrix[bone.y + 0] * weight.yyyy;
    rows[1] += g_localWorldMatrix.lwMatrix[bone.y + 1] * weight.yyyy;
    rows[2] += g_localWorldMatrix.lwMatrix[bone.y + 2] * weight.yyyy;

    rows[0] += g_localWorldMatrix.lwMatrix[bone.z + 0] * weight.zzzz;
    rows[1] += g_localWorldMatrix.lwMatrix[bone.z + 1] * weight.zzzz;
    rows[2] += g_localWorldMatrix.lwMatrix[bone.z + 2] * weight.zzzz;

    rows[0] += g_localWorldMatrix.lwMatrix[bone.w + 0] * weight.wwww;
    rows[1] += g_localWorldMatrix.lwMatrix[bone.w + 1] * weight.wwww;
    rows[2] += g_localWorldMatrix.lwMatrix[bone.w + 2] * weight.wwww;

#elif (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_8FRAME) || \
      (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_NMAP_8FRAME)

    int4 bone = input.blendIndices0;
    float4 weight = input.blendWeight0;

    rows[0] = g_localWorldMatrix.lwMatrix[bone.x + 0] * weight.xxxx;
    rows[1] = g_localWorldMatrix.lwMatrix[bone.x + 1] * weight.xxxx;
    rows[2] = g_localWorldMatrix.lwMatrix[bone.x + 2] * weight.xxxx;

    rows[0] += g_localWorldMatrix.lwMatrix[bone.y + 0] * weight.yyyy;
    rows[1] += g_localWorldMatrix.lwMatrix[bone.y + 1] * weight.yyyy;
    rows[2] += g_localWorldMatrix.lwMatrix[bone.y + 2] * weight.yyyy;

    rows[0] += g_localWorldMatrix.lwMatrix[bone.z + 0] * weight.zzzz;
    rows[1] += g_localWorldMatrix.lwMatrix[bone.z + 1] * weight.zzzz;
    rows[2] += g_localWorldMatrix.lwMatrix[bone.z + 2] * weight.zzzz;

    rows[0] += g_localWorldMatrix.lwMatrix[bone.w + 0] * weight.wwww;
    rows[1] += g_localWorldMatrix.lwMatrix[bone.w + 1] * weight.wwww;
    rows[2] += g_localWorldMatrix.lwMatrix[bone.w + 2] * weight.wwww;

    bone = input.blendIndices1;
    weight = input.blendWeight1;

    rows[0] += g_localWorldMatrix.lwMatrix[bone.x + 0] * weight.xxxx;
    rows[1] += g_localWorldMatrix.lwMatrix[bone.x + 1] * weight.xxxx;
    rows[2] += g_localWorldMatrix.lwMatrix[bone.x + 2] * weight.xxxx;

    rows[0] += g_localWorldMatrix.lwMatrix[bone.y + 0] * weight.yyyy;
    rows[1] += g_localWorldMatrix.lwMatrix[bone.y + 1] * weight.yyyy;
    rows[2] += g_localWorldMatrix.lwMatrix[bone.y + 2] * weight.yyyy;

    rows[0] += g_localWorldMatrix.lwMatrix[bone.z + 0] * weight.zzzz;
    rows[1] += g_localWorldMatrix.lwMatrix[bone.z + 1] * weight.zzzz;
    rows[2] += g_localWorldMatrix.lwMatrix[bone.z + 2] * weight.zzzz;

    rows[0] += g_localWorldMatrix.lwMatrix[bone.w + 0] * weight.wwww;
    rows[1] += g_localWorldMatrix.lwMatrix[bone.w + 1] * weight.wwww;
    rows[2] += g_localWorldMatrix.lwMatrix[bone.w + 2] * weight.wwww;
#endif
}

float3 ModelToWorldPosition(VS_INPUT input)
{
    float4 localPos = float4(input.pos, 1.0f);

#if (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_1FRAME) || \
    (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_NMAP_1FRAME)
    return mul(localPos, g_base.localWorldMatrix);
#else
    float4 rows[3];
    BuildModelWorldMatrix(input, rows);
    return float3(dot(localPos, rows[0]), dot(localPos, rows[1]), dot(localPos, rows[2]));
#endif
}

float3 ModelToWorldNormal(VS_INPUT input)
{
    float4 localNormal = float4(input.norm, 0.0f);

#if (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_1FRAME) || \
    (VERTEX_INPUT == DX_MV1_VERTEX_TYPE_NMAP_1FRAME)
    return normalize(mul(localNormal, g_base.localWorldMatrix));
#else
    float4 rows[3];
    BuildModelWorldMatrix(input, rows);
    return normalize(float3(dot(localNormal, rows[0]), dot(localNormal, rows[1]), dot(localNormal, rows[2])));
#endif
}

float3 WorldToViewPosition(float3 worldPos)
{
    return mul(float4(worldPos, 1.0f), g_base.viewMatrix);
}

float3 WorldToViewNormal(float3 worldNormal)
{
    return normalize(mul(float4(worldNormal, 0.0f), g_base.viewMatrix));
}

float4 ViewToProjection(float3 viewPos)
{
    return mul(float4(viewPos, 1.0f), g_base.projectionMatrix);
}

#endif
