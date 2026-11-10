#include "../Common/GoalFieldVarying.hlsli"

cbuffer GoalFieldParameters : register(b4)
{
    float gTime;
    float gSpeed;
    float gBrightness;
    float gOpacity;
    float gWaveHeight;
    float gWaveFrequency;
    float gRadius;
    float gHeight;
};

float hash21(float2 p)
{
    return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453);
}

float4 main(GoalFieldVarying i) : SV_TARGET
{
    // Cylindrical coordinates derived from model-local position, not UVs.
    float3 p = i.localPos;
    float angle = atan2(p.z, p.x);
    float u = frac(angle / 6.28318530718 + 0.5);
    float v = (p.y + gHeight * 0.5) / max(abs(gHeight), 0.001);
    float t = gTime * gSpeed;

    float streak = pow(saturate(0.5 + 0.5 * sin(
        angle * 17.0 + v * 7.0 - t * 2.0 + sin(v * 11.0 - t) * 1.1)), 12.0);
    float flow = pow(saturate(0.5 + 0.5 * sin(
        v * 28.0 - t * 4.5 + sin(angle * 9.0))), 12.0);

    float2 coord = float2(u * 28.0, v * 11.0 - t * 0.7);
    float2 cell = floor(coord);
    float2 f = abs(frac(coord) - 0.5);
    float square = step(0.973, hash21(cell)) *
        (1.0 - smoothstep(0.20, 0.26, max(f.x, f.y)));

    float edge = pow(1.0 - saturate(v), 20.0) +
                 pow(saturate(v), 20.0);
    float pulse = 0.90 + 0.10 * sin(t * 2.8);
    float energy = (0.14 + streak * 0.45 + flow * 0.27 +
                    square * 1.5 + edge * 1.3) * pulse;

    // Strong yellow tint even at maximum glow. Avoid white clipping.
    float glowMask = (streak * 0.8 + flow * 0.5 +
                      square * 1.5 + edge * 2.0) * pulse;

    // Luminous details brighten the yellow, rather than adding white RGB.
    float glowLevel = (0.65 + glowMask * 0.85) * max(gBrightness, 0.0);
    float level = saturate(glowLevel);
    float3 baseYellow = float3(1.0, 0.78, 0.015);
    float3 highlightYellow = float3(1.0, 0.91, 0.055);
    float3 yellow = lerp(baseYellow, highlightYellow,
                         saturate(glowMask * 0.55));
    float3 color = yellow * level;

    // Retain translucent wall while emphasizing animated highlights.
    float baseAlpha = 0.12 + saturate(gOpacity) * 0.46;
    float lightAlpha = streak * 0.22 + flow * 0.14 + square * 0.52;
    float rimAlpha = saturate(edge) * 0.36;
    float alpha = saturate(baseAlpha + lightAlpha + rimAlpha);
    
    // 黄色の明るさを底上げ
    float3 yellowBoost = float3(1.0, 1.0, 0.0);
    
    // 既存の発光表現を残しながら黄色を追加
    color += yellowBoost * 0.25;
    
    // 白飛びを抑える
    color = saturate(color);

    return float4(color, alpha);
}
