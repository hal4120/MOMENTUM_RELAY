#include "SkyBox.h"

#include "../../../pch.h"

#include "../../../Manager/Shader/ShaderResourceManager.h"

SkyBox::SkyBox(float size) :
	size(size),

	vertexBuffer(-1),
	indexBuffer(-1),

	vertexShader(-1),
	pixelShader(-1),

	texture(-1)
{
}

void SkyBox::Init(void)
{
	const float half = size * 0.5f;

	//--------------------------------------------------
	// 頂点バッファ生成
	//--------------------------------------------------

	// 4頂点 × 6面
	vertexBuffer = CreateVertexBuffer(24, DX_VERTEX_TYPE_NORMAL_3D);

	VERTEX3D* vertices = static_cast<VERTEX3D*>(GetBufferVertexBuffer(vertexBuffer));

	//--------------------------------------------------
	// 各面の頂点
	//--------------------------------------------------

	// 前面
	vertices[0].pos = VGet(-half, half, half);
	vertices[1].pos = VGet(half, half, half);
	vertices[2].pos = VGet(-half, -half, half);
	vertices[3].pos = VGet(half, -half, half);

	// 後面
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

	//--------------------------------------------------
	// UV
	//--------------------------------------------------

	for (int i = 0; i < 6; ++i)
	{
		const int index = i * 4;

		vertices[index + 0].u = 0.0f;
		vertices[index + 0].v = 0.0f;

		vertices[index + 1].u = 1.0f;
		vertices[index + 1].v = 0.0f;

		vertices[index + 2].u = 0.0f;
		vertices[index + 2].v = 1.0f;

		vertices[index + 3].u = 1.0f;
		vertices[index + 3].v = 1.0f;
	}

	//--------------------------------------------------
	// 色
	//--------------------------------------------------

	for (int i = 0; i < 24; ++i)
	{
		vertices[i].dif = GetColorU8(255, 255, 255, 255);
		vertices[i].spc = GetColorU8(0, 0, 0, 0);

		vertices[i].norm = VGet(0.0f, 0.0f, 0.0f);
	}

	UpdateVertexBuffer(vertexBuffer, 0, 24);

	//--------------------------------------------------
	// インデックスバッファ生成
	//--------------------------------------------------

	indexBuffer = CreateIndexBuffer(36, DX_INDEX_TYPE_16BIT);

	unsigned short* indices = static_cast<unsigned short*>(GetBufferIndexBuffer(indexBuffer));

	for (int face = 0; face < 6; ++face)
	{
		const int vertex = face * 4;
		const int index = face * 6;

		// 内側から見えるように時計回り
		indices[index + 0] = vertex + 0;
		indices[index + 1] = vertex + 2;
		indices[index + 2] = vertex + 1;

		indices[index + 3] = vertex + 1;
		indices[index + 4] = vertex + 2;
		indices[index + 5] = vertex + 3;
	}

	UpdateIndexBuffer(indexBuffer, 0, 36);

#pragma region シェーダーハンドルを取得

	auto& shaderManager = ShaderResourceManager::GetIns();

	shaderManager.CreateVertexShader(VERTEX_SHADER_TYPE::SkyBox);
	shaderManager.CreatePixelShader(PIXEL_SHADER_TYPE::SkyBox);

	vertexShader = shaderManager.GetVertexShader(VERTEX_SHADER_TYPE::SkyBox);
	pixelShader = shaderManager.GetPixelShader(PIXEL_SHADER_TYPE::SkyBox);

#pragma endregion

}

void SkyBox::Draw(void)
{
	if (vertexBuffer == -1 || indexBuffer == -1 ||
		vertexShader == -1 || pixelShader == -1) {
		return;
	}

	// 描画設定
	SetUseLighting(false);

	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(false);

	// 深度が最奥でも描画されるようにする
	SetZBufferCmpType(DX_CMP_LESSEQUAL);

	// 内側から描画するため、一旦カリングを無効化
	SetUseBackCulling(false);

	// シェーダー設定
	SetUseVertexShader(vertexShader);
	SetUsePixelShader(pixelShader);

	// 描画
	DrawPrimitiveIndexed3D_UseVertexBuffer(
		vertexBuffer,
		indexBuffer,
		DX_PRIMTYPE_TRIANGLELIST,
		-1,
		false
	);

	// シェーダー解除
	SetUseVertexShader(-1);
	SetUsePixelShader(-1);

	// 描画設定を戻す（通常の設定を仮定）
	SetUseBackCulling(true);
	SetZBufferCmpType(DX_CMP_LESS);

	SetWriteZBuffer3D(true);
	SetUseLighting(true);
}

void SkyBox::Release(void)
{
	vertexShader = -1;
	pixelShader = -1;

	if (vertexBuffer != -1) {
		DeleteVertexBuffer(vertexBuffer);
		vertexBuffer = -1;
	}

	if (indexBuffer != -1) {
		DeleteIndexBuffer(indexBuffer);
		indexBuffer = -1;
	}
}

void SkyBox::SetTexture(int texture)
{
	this->texture = texture;
}