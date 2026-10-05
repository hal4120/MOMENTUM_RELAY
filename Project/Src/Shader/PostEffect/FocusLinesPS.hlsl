#include "PostEffectHeader.hlsli"

cbuffer FocusLinesParam : register(b0)
{
    float time;
    float intensity;
    float speed;
    float lineCount;
};


// ------------------------------------------------------------
// 疑似乱数
// ------------------------------------------------------------
float Random(float seed)
{
    return frac(
        sin(seed * 12.9898f) *
        43758.5453f
    );
}


// ------------------------------------------------------------
// 集中線
// ------------------------------------------------------------
float4 main(PS_INPUT input) : SV_TARGET
{
    uint textureWidth;
    uint textureHeight;

    sceneTexture.GetDimensions(
        textureWidth,
        textureHeight
    );

    float2 screenSize =
        float2(
            textureWidth,
            textureHeight
        );


    // 元画像
    float2 uv = input.uv;

    float4 color =
        GetSceneColor(uv);


    // --------------------------------------------------------
    // 画面中央を原点へ
    // --------------------------------------------------------

    float2 p = uv - 0.5f;

    float aspect =
        screenSize.x /
        screenSize.y;

    p.x *= aspect;


    // 中央からの距離
    float dist = length(p);


    // 角度
    const float PI = 3.14159265f;

    float angle =
        atan2(
            p.y,
            p.x
        );

    float angle01 =
        angle /
        (2.0f * PI) +
        0.5f;


    // --------------------------------------------------------
    // 時間によって「線の配置パターン」そのものを変更
    // --------------------------------------------------------

    float anim =
        time * speed;

    float currentFrame =
        floor(anim);

    float nextFrame =
        currentFrame + 1.0f;

    float frameBlend =
        smoothstep(
            0.0f,
            1.0f,
            frac(anim)
        );


    // --------------------------------------------------------
    // 角度をlineCount個の領域へ分割
    // --------------------------------------------------------

    float count =
        max(
            lineCount,
            1.0f
        );

    float lineValue =
        angle01 * count;

    float lineIndex =
        floor(lineValue);

    float localAngle =
        frac(lineValue);


    // --------------------------------------------------------
    // 現在フレームのランダム値
    // --------------------------------------------------------

    float seedA =
        lineIndex +
        currentFrame * 137.0f;

    float angleRandomA =
        Random(seedA + 11.0f);

    float lengthRandomA =
        Random(seedA + 29.0f);

    float widthRandomA =
        Random(seedA + 47.0f);

    float brightnessRandomA =
        Random(seedA + 71.0f);

    float enableRandomA =
        Random(seedA + 89.0f);


    // --------------------------------------------------------
    // 次フレームのランダム値
    // --------------------------------------------------------

    float seedB =
        lineIndex +
        nextFrame * 137.0f;

    float angleRandomB =
        Random(seedB + 11.0f);

    float lengthRandomB =
        Random(seedB + 29.0f);

    float widthRandomB =
        Random(seedB + 47.0f);

    float brightnessRandomB =
        Random(seedB + 71.0f);

    float enableRandomB =
        Random(seedB + 89.0f);


    // --------------------------------------------------------
    // ランダム値を時間補間
    // --------------------------------------------------------

    float angleRandom =
        lerp(
            angleRandomA,
            angleRandomB,
            frameBlend
        );

    float lengthRandom =
        lerp(
            lengthRandomA,
            lengthRandomB,
            frameBlend
        );

    float widthRandom =
        lerp(
            widthRandomA,
            widthRandomB,
            frameBlend
        );

    float brightnessRandom =
        lerp(
            brightnessRandomA,
            brightnessRandomB,
            frameBlend
        );

    float enableRandom =
        lerp(
            enableRandomA,
            enableRandomB,
            frameBlend
        );


    // --------------------------------------------------------
    // 線の角度位置
    //
    // ここが以前との大きな違い。
    // 線の中心位置そのものをランダムに移動させる。
    // --------------------------------------------------------

    float lineCenter =
        angleRandom;

    float lineDistance =
        abs(
            localAngle -
            lineCenter
        );

    // 0～1の境界を跨ぐ場合
    lineDistance =
        min(
            lineDistance,
            1.0f - lineDistance
        );


    // --------------------------------------------------------
    // 線の太さ
    // --------------------------------------------------------

    float lineWidth =
        lerp(
            0.015f,
            0.10f,
            widthRandom
        );

    float angleMask =
        1.0f -
        smoothstep(
            lineWidth * 0.3f,
            lineWidth,
            lineDistance
        );


    // --------------------------------------------------------
    // 線の長さ
    //
    // 外側から中央方向へ伸びる。
    // --------------------------------------------------------

    float innerRadius =
    lerp(
        0.40f,
        0.58f,
        lengthRandom
    );

    float outerRadius =
        lerp(
            0.65f,
            1.10f,
            Random(seedA + 103.0f)
        );


    // 内側境界
    float innerMask =
        smoothstep(
            innerRadius,
            innerRadius + 0.04f,
            dist
        );


    // 外側境界
    float outerMask =
        1.0f -
        smoothstep(
            outerRadius,
            outerRadius + 0.10f,
            dist
        );


    float radialMask =
        innerMask *
        outerMask;


    // --------------------------------------------------------
    // 外側ほど太くする
    //
    // 放射状の三角形っぽい集中線になる
    // --------------------------------------------------------

    float radialWidth =
        saturate(
            (
                dist -
                innerRadius
            ) * 3.0f
        );

    float wideAngleMask =
        1.0f -
        smoothstep(
            lineWidth *
            lerp(
                0.25f,
                1.8f,
                radialWidth
            ),

            lineWidth *
            lerp(
                0.5f,
                2.4f,
                radialWidth
            ),

            lineDistance
        );


    // --------------------------------------------------------
    // 一部の線を消す
    //
    // 全方向に均等に並んでいる感じをなくす
    // --------------------------------------------------------

    float appearMask =
        smoothstep(
            0.30f,
            0.50f,
            enableRandom
        );


    // --------------------------------------------------------
    // 明るさ
    // --------------------------------------------------------

    float brightness =
        lerp(
            0.35f,
            1.0f,
            brightnessRandom
        );


    // 細線 + 太線を少し混ぜる
    float lineMask =
        max(
            angleMask * 0.4f,
            wideAngleMask
        );


    // --------------------------------------------------------
    // 最終的な集中線
    // --------------------------------------------------------

    float focusLine =
        lineMask *
        radialMask *
        appearMask *
        brightness *
        intensity;


    // 白い集中線を加算
    color.rgb += focusLine.xxx;

    return color;
}