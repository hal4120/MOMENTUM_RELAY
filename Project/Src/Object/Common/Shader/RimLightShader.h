#pragma once

#include "ShaderBase.h"

#include "../../../Manager/Shader/ShaderTypeDefine.h"

class RimLightShader : public ShaderBase
{
public:
	RimLightShader(SHADER_VERTEX_TYPE vertexType = SHADER_VERTEX_TYPE::Frame8) :
		ShaderBase(),
		vertexType(vertexType)
	{
	}
	~RimLightShader()override = default;

	/// シェーダーの初期化
	void Init(void);

private:
	// 頂点タイプ
	SHADER_VERTEX_TYPE vertexType;
};