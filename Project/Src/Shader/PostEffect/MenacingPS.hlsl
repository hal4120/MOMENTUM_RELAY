#include "PostEffectHeader.hlsli"

Texture2D glyphTexture : register(t1);
SamplerState glyphSampler : register(s1);

cbuffer MenacingParam : register(b0)
{
    float time;
    float intensity;
    float shakeStrength;
    float moveSpeed;
};

float Random(float n)
{
    return frac(sin(n * 127.1f) * 43758.5453f);
}

float2 Random2(float n)
{
    return float2(
        Random(n * 13.1f),
        Random(n * 31.7f)
    );
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.uv;
    float4 scene = GetSceneColor(uv);

    uint width, height;
    sceneTexture.GetDimensions(width, height);
    float aspect = (float) width / max((float) height, 1.0f);

    float3 result = scene.rgb;

    // ゴの表示数
    static const int GLYPH_COUNT = 12;

    [unroll]
    for (int i = 0; i < GLYPH_COUNT; ++i)
    {
        float id = (float) i;

        // 各文字の基本位置
        float2 position = Random2(id + 10.0f);

        // 文字ごとにゆっくり漂う
        position += float2(
            sin(time * moveSpeed * 0.5f + id * 2.3f),
            cos(time * moveSpeed * 0.4f + id * 1.7f)
        ) * 0.015f;

        // 時間ごとに不規則な震え
        float shakeFrame = floor(time * 30.0f);

        float2 shake =
            Random2(id * 17.0f + shakeFrame * 23.0f) - 0.5f;

        position += shake * shakeStrength * 0.015f;

        // 文字ごとにサイズを変える
        float scale = lerp(
            0.07f,
            0.20f,
            Random(id + 43.0f)
        );

        // 文字ごとに傾ける
        float angle =
            (Random(id + 71.0f) - 0.5f) * 0.6f;

        float c = cos(angle);
        float s = sin(angle);

        // アスペクト比補正
        float2 p = uv - position;
        p.x *= aspect;

        float2 rotated = float2(
            c * p.x + s * p.y,
            -s * p.x + c * p.y
        );

        // 文字テクスチャのUV
        float2 glyphUV = rotated / scale + 0.5f;

        if (all(glyphUV >= 0.0f) && all(glyphUV <= 1.0f))
        {
            float4 glyph =
                glyphTexture.Sample(glyphSampler, glyphUV);

            // PNGのアルファを文字マスクとして使用
            float alpha = glyph.a * saturate(intensity);

            // 紫色の文字
            float3 glyphColor = float3(
                0.48f,
                0.12f,
                0.65f
            );

            result = lerp(
                result,
                glyphColor,
                alpha
            );
        }
    }

    return float4(result, scene.a);
}