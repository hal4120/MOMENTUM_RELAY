struct SkyBoxInput
{
    float4 svPos : SV_POSITION;
    float3 direction : TEXCOORD0;
};

float4 main(SkyBoxInput input) : SV_TARGET
{
    float3 dir = normalize(input.direction);

    // •ûŒü‚ðRGB‚Ö•ÏŠ·
    float3 color = dir * 0.5f + 0.5f;

    return float4(color, 1.0f);
}