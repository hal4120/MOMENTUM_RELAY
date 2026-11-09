#include "../Common/VertexToPixelHeader.hlsli"
#define PS_INPUT VertexToPixelLit
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

cbuffer GoalFieldParameters : register(b4)
{
    float gTime;
    float gSpeed;
    float gBrightness;
    float gOpacity;
};
float hash21(float2 p) { return frac(sin(dot(p, float2(127.1,311.7))) * 43758.5453); }
float4 main(PS_INPUT i) : SV_TARGET
{
    float2 uv = i.uv;
    float t = gTime * gSpeed;
    // UV-based animation: assumes cylindrical wall UVs with V increasing downward.
    float movingV = uv.y + t * 0.24;
    float wave = sin(uv.x * 35.0 + sin(movingV * 12.0) * 1.8 + t * 1.5);
    float strands = pow(saturate(0.5 + 0.5 * wave), 8.0);
    float sweep = pow(saturate(0.5 + 0.5 * sin(movingV * 24.0 + sin(uv.x * 16.0))), 9.0);
    float2 cells = floor(float2(uv.x * 32.0, movingV * 12.0));
    float2 local = frac(float2(uv.x * 32.0, movingV * 12.0));
    float sparkle = step(0.965, hash21(cells)) * step(0.15, local.x) * step(local.x, 0.85) * step(0.15, local.y) * step(local.y, 0.85);
    float rings = pow(1.0 - saturate(uv.y) , 25.0) + pow(saturate(uv.y), 25.0);
    float pulse = 0.88 + 0.12 * sin(t * 2.7);
    float energy = (0.20 + strands * 0.32 + sweep * 0.25 + sparkle * 1.3 + rings * 1.1) * pulse;
    float3 gold = lerp(float3(1.0, 0.32, 0.015), float3(1.0, 0.94, 0.35), saturate(energy * 1.1));
    float3 color = gold * (0.55 + energy * 1.8) * gBrightness;
    float alpha = saturate(gOpacity * (0.35 + energy * 0.8) + sparkle * 0.35 + rings * 0.25);
    return float4(color, alpha);
}
