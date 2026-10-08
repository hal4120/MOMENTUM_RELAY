// ‰_‚Ìƒpƒ‰ƒ[ƒ^
cbuffer CloudParam : register(b4)
{
    float time;
    float cloudScale;
    float cloudSpeed;
    float cloudCoverage;

    float cloudOpacity;
    float3 padding;
};

struct SkyBoxInput
{
    float4 svPos : SV_POSITION;
    float3 direction : TEXCOORD0;
};

// ‹^——”
float Hash(float2 p)
{
    return frac(
        sin(dot(p, float2(127.1f, 311.7f))) *
        43758.5453f
    );
}

// 2D Value Noise
float Noise(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);

    // ŠŠ‚ç‚©‚È•âŠÔ
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

// 3‘w‚ÌƒmƒCƒY‚ğ‡¬
float FBM(float2 p)
{
    float value = 0.0f;
    float amplitude = 0.5f;

    for (int i = 0; i < 3; ++i)
    {
        value += Noise(p) * amplitude;

        p *= 2.0f;
        amplitude *= 0.5f;
    }

    return value / 0.875f;
}

float4 main(SkyBoxInput input) : SV_TARGET
{
    float3 dir = normalize(input.direction);

    float height = dir.y;

    // ==============================
    // ‹ó‚ÌƒOƒ‰ƒf[ƒVƒ‡ƒ“
    // ==============================

    float3 zenithColor = float3(
        0.08f, 0.35f, 0.85f
    );

    float3 horizonColor = float3(
        0.65f, 0.85f, 1.0f
    );

    float3 bottomColor = float3(
        0.85f, 0.90f, 0.95f
    );

    float3 skyColor;

    if (height >= 0.0f)
    {
        float t = pow(saturate(height), 0.65f);

        skyColor = lerp(
            horizonColor,
            zenithColor,
            t
        );
    }
    else
    {
        skyColor = lerp(
            horizonColor,
            bottomColor,
            saturate(-height)
        );
    }

    // ==============================
    // ‰_‚Ì¶¬
    // ==============================

    // ’n•½ü•t‹ß‚ÅUV‚ª‹É’[‚É‘å‚«‚­‚È‚ç‚È‚¢‚æ‚¤‚É‚·‚é
    float safeHeight = max(height, 0.08f);

    // ã‹ó‚Ì‰¼‘z•½–Ê‚Ö“Š‰e
    float2 cloudUV = dir.xz / safeHeight;

    cloudUV *= cloudScale;

    // ŠÔŒo‰ß‚É‚æ‚é‰_‚ÌˆÚ“®
    cloudUV += float2(
        time * cloudSpeed,
        time * cloudSpeed * 0.35f
    );

    // ƒmƒCƒY¶¬
    float noiseValue = FBM(cloudUV);

    // ‰_‚Ì—ÖŠs
    float cloud = smoothstep(
        cloudCoverage - 0.12f,
        cloudCoverage + 0.12f,
        noiseValue
    );

    // ’n•½ü•t‹ß‚Å‚Í‰_‚ğÁ‚·
    float horizonFade = smoothstep(
        0.03f,
        0.25f,
        height
    );

    cloud *= horizonFade;

    // ‰_‚Ì“§–¾“x
    cloud *= cloudOpacity;

    // ==============================
    // ‰_‚ÌF
    // ==============================

    float3 cloudShadow = float3(
        0.72f, 0.80f, 0.88f
    );

    float3 cloudLight = float3(
        1.0f, 1.0f, 1.0f
    );

    // ƒmƒCƒY‚É‰‚¶‚Ä‰_‚É”Z’W‚ğ‚Â‚¯‚é
    float3 cloudColor = lerp(
        cloudShadow,
        cloudLight,
        saturate(noiseValue * 1.5f)
    );

    // ‹ó‚Æ‰_‚ğ‡¬
    float3 finalColor = lerp(
        skyColor,
        cloudColor,
        saturate(cloud)
    );

    return float4(finalColor, 1.0f);
}