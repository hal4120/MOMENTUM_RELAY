// ========================================
// Pixel Shader Input
// ========================================

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 diffuse : COLOR0;
    float2 uv : TEXCOORD0;
    float2 suv : TEXCOORD1;
};

// ========================================
// Scene Texture
// ========================================

SamplerState sceneSampler : register(s0);
Texture2D sceneTexture : register(t0);


// ========================================
// Scene Color
// ========================================

// 指定したUV座標のシーンカラーを取得
float4 GetSceneColor(float2 uv)
{
    return sceneTexture.Sample(sceneSampler, uv);
}

// 現在のピクセル位置のシーンカラーを取得
float4 GetSceneColor(PS_INPUT input)
{
    return sceneTexture.Sample(sceneSampler, input.uv);
}

// ========================================
// Utility
// ========================================

// 0.0 ～ 1.0 の疑似乱数を生成
float Rand(float2 value)
{
    return frac(
        sin(
            dot(
                value,
                float2(12.9898f, 78.233f)
            )
        ) * 43758.5453f
    );
}