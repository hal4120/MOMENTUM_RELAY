#include "PostEffectHeader.hlsli"

// ============================================================
// 定数バッファ
// ============================================================

cbuffer PurpleFlameParam : register(b0)
{
    float time;
    float intensity;
    float flameHeight;
    float flameSpeed;
};


// ============================================================
// 疑似乱数
// ============================================================

float Hash(float2 p)
{
    return frac(
        sin(dot(p, float2(127.1f, 311.7f))) *
        43758.5453f
    );
}


// ============================================================
// 2D Value Noise
// ============================================================

float Noise(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);

    f = f * f * (3.0f - 2.0f * f);

    float a = Hash(i);
    float b = Hash(i + float2(1.0f, 0.0f));
    float c = Hash(i + float2(0.0f, 1.0f));
    float d = Hash(i + float2(1.0f, 1.0f));

    return lerp(
        lerp(a, b, f.x),
        lerp(c, d, f.x),
        f.y
    );
}


// ============================================================
// Fractal Brownian Motion
// 複数のノイズを重ねて炎らしい複雑な形状を作る
// ============================================================

float FBM(float2 p)
{
    float value = 0.0f;
    float amplitude = 0.5f;

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        value += Noise(p) * amplitude;

        p *= 2.0f;
        amplitude *= 0.5f;
    }

    return value;
}


// ============================================================
// ピクセルシェーダー
// ============================================================

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 uv = input.uv;

    // 元のゲーム画面
    float4 sceneColor = GetSceneColor(uv);

    // --------------------------------------------------------
    // 画面下端を原点とした高さ
    // --------------------------------------------------------

    // 画面下端 = 0
    // 画面上端 = 1
    float height = 1.0f - uv.y;

    float safeHeight = max(flameHeight, 0.001f);

    // 炎の高さを正規化
    float normalizedHeight = height / safeHeight;

    // --------------------------------------------------------
    // 時間でノイズを上方向へ流す
    // --------------------------------------------------------

    float t = time * flameSpeed;

    float2 noiseUV = float2(
        uv.x * 7.0f,
        normalizedHeight * 3.0f - t
    );

    // 大きな炎の揺らぎ
    float largeNoise = FBM(noiseUV);

    // 細かい炎の揺らぎ
    float detailNoise = FBM(
        noiseUV * 2.8f +
        float2(13.7f, -t * 0.7f)
    );

    // --------------------------------------------------------
    // 炎の輪郭
    // --------------------------------------------------------

    // ノイズによって炎の先端を上下させる
    float flameTop =
        0.55f +
        largeNoise * 0.55f +
        detailNoise * 0.20f;

    // 高さがflameTopを超えると消える
    float flameMask =
        1.0f -
        smoothstep(
            flameTop - 0.15f,
            flameTop + 0.10f,
            normalizedHeight
        );

    // --------------------------------------------------------
    // 炎の内部模様
    // --------------------------------------------------------

    float innerNoise = FBM(
        float2(
            uv.x * 12.0f,
            normalizedHeight * 5.0f - t * 1.5f
        )
    );

    // 炎の筋を作る
    float flameDetail =
        smoothstep(
            0.25f,
            0.85f,
            innerNoise
        );

    // 上に行くほど薄くする
    float heightFade =
        1.0f -
        smoothstep(
            0.1f,
            1.15f,
            normalizedHeight
        );

    // --------------------------------------------------------
    // 炎の強度
    // --------------------------------------------------------

    float flame =
        flameMask *
        heightFade *
        (0.45f + flameDetail * 0.55f);

    flame *= saturate(intensity);

    // --------------------------------------------------------
    // 紫色グラデーション
    // --------------------------------------------------------

    // 外側：暗い紫
    float3 outerColor = float3(
        0.18f,
        0.01f,
        0.32f
    );

    // 中間：紫
    float3 middleColor = float3(
        0.55f,
        0.05f,
        0.85f
    );

    // 内側：明るいピンク紫
    float3 innerColor = float3(
        1.0f,
        0.35f,
        0.95f
    );

    float3 flameColor = lerp(
        outerColor,
        middleColor,
        saturate(flameDetail * 1.5f)
    );

    flameColor = lerp(
        flameColor,
        innerColor,
        pow(saturate(flameDetail), 3.0f)
    );

    // --------------------------------------------------------
    // 発光
    // --------------------------------------------------------

    float glow = flame * flame;

    flameColor += innerColor * glow * 0.35f;

    // --------------------------------------------------------
    // 元画面と合成
    // --------------------------------------------------------

    // 加算合成寄りの炎表現
    float3 result =
        sceneColor.rgb +
        flameColor * flame;

    return float4(
        saturate(result),
        sceneColor.a
    );
}