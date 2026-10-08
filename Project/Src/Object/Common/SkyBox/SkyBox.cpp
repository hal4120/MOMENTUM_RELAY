#include "SkyBox.h"

#include "../../../Manager/Shader/ShaderResourceManager.h"

SkyBox::SkyBox(float size) :
    size(size),
    vertices{},
    indices{},
    vertexShader(-1),
    pixelShader(-1)
{
}

void SkyBox::Init(void)
{
    const float half = size * 0.5f;

    // 前面
    vertices[0].pos = VGet(-half, half, half);
    vertices[1].pos = VGet(half, half, half);
    vertices[2].pos = VGet(-half, -half, half);
    vertices[3].pos = VGet(half, -half, half);

    // 背面
    vertices[4].pos = VGet(half, half, -half);
    vertices[5].pos = VGet(-half, half, -half);
    vertices[6].pos = VGet(half, -half, -half);
    vertices[7].pos = VGet(-half, -half, -half);

    // 左面
    vertices[8].pos = VGet(-half, half, -half);
    vertices[9].pos = VGet(-half, half, half);
    vertices[10].pos = VGet(-half, -half, -half);
    vertices[11].pos = VGet(-half, -half, half);

    // 右面
    vertices[12].pos = VGet(half, half, half);
    vertices[13].pos = VGet(half, half, -half);
    vertices[14].pos = VGet(half, -half, half);
    vertices[15].pos = VGet(half, -half, -half);

    // 上面
    vertices[16].pos = VGet(-half, half, -half);
    vertices[17].pos = VGet(half, half, -half);
    vertices[18].pos = VGet(-half, half, half);
    vertices[19].pos = VGet(half, half, half);

    // 下面
    vertices[20].pos = VGet(-half, -half, half);
    vertices[21].pos = VGet(half, -half, half);
    vertices[22].pos = VGet(-half, -half, -half);
    vertices[23].pos = VGet(half, -half, -half);

    // 頂点情報
    for (int i = 0; i < VERTEX_NUM; ++i)
    {
        vertices[i].norm = VGet(0.0f, 0.0f, 0.0f);

        vertices[i].dif = GetColorU8(255, 255, 255, 255);
        vertices[i].spc = GetColorU8(0, 0, 0, 0);

        vertices[i].u = 0.0f;
        vertices[i].v = 0.0f;
        vertices[i].su = 0.0f;
        vertices[i].sv = 0.0f;
    }

    // インデックス設定
    for (int face = 0; face < 6; ++face)
    {
        const int vertex = face * 4;
        const int index = face * 6;

        indices[index + 0] = vertex + 0;
        indices[index + 1] = vertex + 2;
        indices[index + 2] = vertex + 1;

        indices[index + 3] = vertex + 1;
        indices[index + 4] = vertex + 2;
        indices[index + 5] = vertex + 3;
    }

    // シェーダー取得
    auto& shaderManager = ShaderResourceManager::GetIns();

    shaderManager.CreateVertexShader(VERTEX_SHADER_TYPE::SkyBox);
    shaderManager.CreatePixelShader(PIXEL_SHADER_TYPE::SkyBox);

    vertexShader = shaderManager.GetVertexShader(
        VERTEX_SHADER_TYPE::SkyBox
    );

    pixelShader = shaderManager.GetPixelShader(
        PIXEL_SHADER_TYPE::SkyBox
    );
}

void SkyBox::Draw(void)
{
    if (vertexShader == -1 || pixelShader == -1)
    {
        return;
    }

    // 描画設定
    SetUseLighting(false);

    SetUseZBuffer3D(true);
    SetWriteZBuffer3D(false);
    SetZBufferCmpType(DX_CMP_LESSEQUAL);

    // SkyBoxは内側から描画する
    SetUseBackCulling(false);

    // シェーダー設定
    SetUseVertexShader(vertexShader);
    SetUsePixelShader(pixelShader);

    // SkyBox描画
    int result = DrawPolygonIndexed3DToShader(
        vertices.data(),
        VERTEX_NUM,
        indices.data(),
        POLYGON_NUM
    );

    // シェーダー解除
    SetUseVertexShader(-1);
    SetUsePixelShader(-1);

    // 描画設定を戻す
    SetUseBackCulling(true);

    SetZBufferCmpType(DX_CMP_LESS);
    SetWriteZBuffer3D(true);

    SetUseLighting(true);
}

void SkyBox::Release(void)
{
    // シェーダーの実体はShaderResourceManagerが管理する
    vertexShader = -1;
    pixelShader = -1;
}