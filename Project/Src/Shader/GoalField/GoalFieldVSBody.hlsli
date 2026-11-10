#include "../Common/GoalFieldVarying.hlsli"
#define VS_OUTPUT GoalFieldVarying
#include "../Common/Vertex/VertexShader3DHeader.hlsli"
#include "../Common/Vertex/ModelVertexTransform.hlsli"
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
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT o = (VS_OUTPUT)0;
    float3 p = input.pos;
    float angle = atan2(p.z, p.x);
    float phase = gTime * gSpeed;
    float ripple = sin(angle * 7.0 - phase * 2.2 + p.y * gWaveFrequency);
    ripple += 0.35 * sin(angle * 13.0 + phase * 1.6 - p.y * gWaveFrequency * 0.6);
    // Preserve both circular edges; only the middle of the wall gently ripples.
    float h = saturate((p.y + gHeight * 0.5) / max(abs(gHeight), 0.001));
    float envelope = pow(saturate(sin(h * 3.14159265)), 2.0);
    float displacement = ripple * gWaveHeight * 0.25 * envelope;
    // Vertical motion in model local space (independent of UVs).
    float3 displacedLocal = p + float3(0.0, displacement, 0.0);
    // Transform displacement via the model's own vertex-to-world path (supports skinned variants).
    VS_INPUT moved = input;
    moved.pos = displacedLocal;
    float3 wp = ModelToWorldPosition(moved);
    float3 vp = WorldToViewPosition(wp);
    o.svPos = ViewToProjection(vp);
    o.viewPos = vp;
    o.viewNormal = WorldToViewNormal(ModelToWorldNormal(input));
    o.localPos = displacedLocal;
    o.uv = input.uv0.xy;
    return o;
}
