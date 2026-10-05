#pragma once

// 頂点シェーダーの種類
enum class VERTEX_SHADER_TYPE
{
    // デフォルト
    Default1Frame,
    Default4Frame,
    Default8Frame,
    DefaultNMap1Frame,
    DefaultNMap4Frame,
    DefaultNMap8Frame,

    // リムライト
    RimLight1Frame,
    RimLight4Frame,
    RimLight8Frame,
    RimLightNMap1Frame,
    RimLightNMap4Frame,
    RimLightNMap8Frame,

	// 水面
    Water,
};

// ピクセルシェーダーの種類
enum class PIXEL_SHADER_TYPE
{
    // デフォルト
    Default,

    // リムライト
    RimLight,

    // 水面
    Water,

#pragma region ポストエフェクト

	// グレースケールポストエフェクト
    GrayScale,

    // ブラウン管ポストエフェクト
    CRT,

    // 集中線ポストエフェクト
    FocusLines,

#pragma endregion
};

// 頂点タイプ
enum SHADER_VERTEX_TYPE
{
    Frame1,
    Frame4,
    Frame8,
    NMapFrame1,
    NMapFrame4,
    NMapFrame8,
};