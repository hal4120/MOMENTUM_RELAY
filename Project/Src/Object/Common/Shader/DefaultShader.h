#pragma once

#include "ShaderBase.h"

#include "../../../Manager/Shader/ShaderTypeDefine.h"

class DefaultShader : public ShaderBase
{
public:
	// 頂点タイプを指定する
	DefaultShader(SHADER_VERTEX_TYPE vertexType = SHADER_VERTEX_TYPE::Frame8) :
		ShaderBase(),
		vertexType(vertexType)
	{
	}
	~DefaultShader()override = default;

	/// シェーダーの初期化
	void Init(void);

private:
	// 頂点タイプ
	SHADER_VERTEX_TYPE vertexType;
};