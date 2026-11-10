#ifndef GOAL_FIELD_VARYING_HLSLI
#define GOAL_FIELD_VARYING_HLSLI
struct GoalFieldVarying
{
    float4 svPos : SV_POSITION;
    float3 localPos : TEXCOORD0;
    float3 viewPos : TEXCOORD1;
    float3 viewNormal : TEXCOORD2;
    float2 uv : TEXCOORD3;
};
#endif
