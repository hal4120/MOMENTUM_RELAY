#include "PostEffectHeader.hlsli"


cbuffer UnderwaterParam : register(b0)
{
    // 経過時間
    float time;

    // 水中エフェクト全体の強さ
    float intensity;

    // 揺れの強さ
    float distortionStrength;

    // 揺れの速度
    float distortionSpeed;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.uv;


    // ========================================================
    // 水中の揺らぎ
    // ========================================================

    // 横方向の波
    float waveX =
        sin(
            uv.y * 35.0f +
            time * distortionSpeed
        );

    // 縦方向の波
    float waveY =
        sin(
            uv.x * 27.0f -
            time * distortionSpeed * 0.8f
        );


    // 少し周波数の違う波を混ぜる
    waveX +=
        sin(
            uv.y * 73.0f -
            time * distortionSpeed * 1.3f
        ) * 0.35f;

    waveY +=
        sin(
            uv.x * 61.0f +
            time * distortionSpeed * 1.1f
        ) * 0.35f;


    // UVを歪ませる
    float2 distortedUV = uv;

    distortedUV.x +=
        waveX *
        distortionStrength *
        intensity;

    distortedUV.y +=
        waveY *
        distortionStrength *
        intensity;


    // 画面外を参照しないようにする
    distortedUV =
        saturate(distortedUV);


    // ========================================================
    // 元画面取得
    // ========================================================

    float4 color =
        GetSceneColor(distortedUV);


    // ========================================================
    // 水中色
    // ========================================================

    // 青緑系
    float3 underwaterColor =
        float3(
            0.05f,
            0.45f,
            0.65f
        );


    // 元の色を少し暗くする
    float luminance =
        dot(
            color.rgb,
            float3(
                0.299f,
                0.587f,
                0.114f
            )
        );


    // 暗い場所ほど水中色を強くする
    float waterAmount =
        saturate(
            0.25f +
            (1.0f - luminance) * 0.25f
        );


    float3 underwater =
        lerp(
            color.rgb,
            underwaterColor,
            waterAmount
        );


    // ========================================================
    // 水中では赤が減衰しやすい感じを簡易表現
    // ========================================================

    underwater.r *= 0.72f;
    underwater.g *= 0.95f;
    underwater.b *= 1.08f;


    // ========================================================
    // 画面端を少し暗くする
    // ========================================================

    float2 centerUV =
        uv - 0.5f;

    float distanceFromCenter =
        length(centerUV);


    float vignette =
        1.0f -
        smoothstep(
            0.30f,
            0.72f,
            distanceFromCenter
        ) * 0.30f;


    underwater *= vignette;


    // ========================================================
    // 元画像と水中表現を合成
    // ========================================================

    float3 finalColor =
        lerp(
            color.rgb,
            underwater,
            saturate(intensity)
        );


    return float4(
        finalColor,
        color.a
    );
}