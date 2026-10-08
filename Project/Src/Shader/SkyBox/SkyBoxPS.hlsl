struct SkyBoxInput
{
    float4 svPos : SV_POSITION;
    float3 direction : TEXCOORD0;
};

float4 main(SkyBoxInput input) : SV_TARGET
{
    float3 dir = normalize(input.direction);

    // 上下方向
    float height = dir.y;

    // 天頂の色
    float3 zenithColor = float3(
        0.08f,
        0.35f,
        0.85f
    );

    // 地平線の色
    float3 horizonColor = float3(
        0.65f,
        0.85f,
        1.0f
    );

    // 地平線より下の色
    float3 bottomColor = float3(
        0.85f,
        0.90f,
        0.95f
    );

    float3 color;

    if (height >= 0.0f)
    {
        float t = saturate(height);

        // グラデーションを調整
        t = pow(t, 0.65f);

        color = lerp(
            horizonColor,
            zenithColor,
            t
        );
    }
    else
    {
        float t = saturate(-height);

        color = lerp(
            horizonColor,
            bottomColor,
            t
        );
    }

    return float4(color, 1.0f);
}