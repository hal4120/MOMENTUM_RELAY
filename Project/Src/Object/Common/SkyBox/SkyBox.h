#pragma once

#include <array>
#include "../../../pch.h"

class SkyBox
{
public:

    explicit SkyBox(float size = 1000.0f);
    ~SkyBox() = default;

    void Init(void);
    void Update(void);
    void Draw(void);
    void Release(void);

private:

    static constexpr int VERTEX_NUM = 24;
    static constexpr int INDEX_NUM = 36;
    static constexpr int POLYGON_NUM = 12;

    // ピクセルシェーダー用定数
    struct alignas(16) CloudParam
    {
        float time;
        float cloudScale;
        float cloudSpeed;
        float cloudCoverage;

        float cloudOpacity;
        float padding[3];
    };

    float size;

    std::array<VERTEX3DSHADER, VERTEX_NUM> vertices;
    std::array<unsigned short, INDEX_NUM> indices;

    int vertexShader;
    int pixelShader;

    int cloudConstantBuffer;

    CloudParam cloudParam;
};