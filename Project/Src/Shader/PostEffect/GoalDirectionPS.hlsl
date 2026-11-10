Texture2D sourceTexture : register(t0);
Texture2D textTexture : register(t1);
SamplerState sourceSampler : register(s0);
SamplerState textSampler : register(s1);

cbuffer GoalDirectionParameters : register(b0)
{
    float gTime;
    float gIntensity;
    float gFlashSpeed;
    float gPadding;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

float4 main(PSInput i) : SV_TARGET
{
    float3 scene = sourceTexture.Sample(sourceSampler, i.uv).rgb;
    float4 text = textTexture.Sample(textSampler, i.uv);

    // Blink the complete text, but preserve the PNG's white fill and black outline.
    float wave = 0.5 + 0.5 * sin(gTime * gFlashSpeed * 6.2831853);
    float flash = 0.55 + 0.45 * wave * wave;
    float opacity = saturate(text.a * flash * gIntensity);

    // Use the original image colors. Recoloring all opaque pixels yellow
    // would destroy the black outline and make the lettering hard to read.
    float3 result = lerp(scene, text.rgb, opacity);

    // Only illuminate yellow pixels, not the white letters or black outline.
    float yellowMask = saturate((text.r - text.b - 0.12) * 2.0);
    float glow = yellowMask * text.a * flash * 0.06 * saturate(gIntensity);
    result += float3(1.0, 0.85, 0.0) * glow;

    return float4(saturate(result), 1.0);
}
