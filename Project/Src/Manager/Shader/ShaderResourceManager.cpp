#include "ShaderResourceManager.h"

#include <stdexcept>

#include "../../pch.h"

ShaderResourceManager* ShaderResourceManager::ins = nullptr;

void ShaderResourceManager::CreateVertexShader(VERTEX_SHADER_TYPE type)
{
	// 既に生成済みのシェーダーの場合は生成しない
	if (vertexShaderList.contains(type)) { return; }

	// テーブルに指定されたパスが存在しない場合は生成しない
	if (!VERTEX_SHADER_NAME_TABLE.contains(type)) { 
		// エラー処理
		throw std::runtime_error("指定された頂点シェーダーの名前指定が存在しません");
	}

	// シェーダーを読み込み
	vertexShaderList[type] = LoadVertexShader(ShaderNameToPath(VERTEX_SHADER_NAME_TABLE.at(type)).c_str());
}

int ShaderResourceManager::GetVertexShader(VERTEX_SHADER_TYPE type)
{
	// 指定されたシェーダーが生成されていない場合はエラーを返す
	if (!vertexShaderList.contains(type)) {
		// エラー処理
		throw std::runtime_error("取得を試みた頂点シェーダーが生成されていません");
	}

	// シェーダーを返す
	return vertexShaderList.at(type);
}

void ShaderResourceManager::ReleaseVertexShader(VERTEX_SHADER_TYPE type)
{
	// 指定されたシェーダーが生成されていない場合は処理しない
	if (!vertexShaderList.contains(type)) { return; }

	// シェーダーを解放
	DeleteShader(vertexShaderList.at(type));

	// テーブルから削除
	vertexShaderList.erase(type);
}

void ShaderResourceManager::ReleaseAllVertexShader(void)
{
	// すべてのシェーダーを解放
	for (auto& shader : vertexShaderList) { DeleteShader(shader.second); }

	// テーブルをクリア
	vertexShaderList.clear();
}

void ShaderResourceManager::CreatePixelShader(PIXEL_SHADER_TYPE type)
{
	// 既に生成済みのシェーダーの場合は生成しない
	if (pixelShaderList.contains(type)) { return; }

	// テーブルに指定されたパスが存在しない場合は生成しない
	if (!PIXEL_SHADER_NAME_TABLE.contains(type)) { 
		// エラー処理
		throw std::runtime_error("指定されたピクセルシェーダーの名前指定が存在しません");
	}

	// シェーダーを読み込み
	pixelShaderList[type] = LoadPixelShader(ShaderNameToPath(PIXEL_SHADER_NAME_TABLE.at(type)).c_str());
}

int ShaderResourceManager::GetPixelShader(PIXEL_SHADER_TYPE type)
{
	// 指定されたシェーダーが生成されていない場合はエラーを返す
	if (!pixelShaderList.contains(type)) {
		// エラー処理
		throw std::runtime_error("取得を試みたピクセルシェーダーが生成されていません");
	}

	// シェーダーを返す
	return pixelShaderList.at(type);
}

void ShaderResourceManager::ReleasePixelShader(PIXEL_SHADER_TYPE type)
{
	// 指定されたシェーダーが生成されていない場合は処理しない
	if (!pixelShaderList.contains(type)) { return; }

	// シェーダーを解放
	DeleteShader(pixelShaderList.at(type));

	// テーブルから削除
	pixelShaderList.erase(type);
}

void ShaderResourceManager::ReleaseAllPixelShader(void)
{
	// すべてのシェーダーを解放
	for (auto& shader : pixelShaderList) { DeleteShader(shader.second); }

	// テーブルをクリア
	pixelShaderList.clear();
}

void ShaderResourceManager::ReleaseAllShader(void)
{
	ReleaseAllVertexShader();
	ReleaseAllPixelShader();
}
