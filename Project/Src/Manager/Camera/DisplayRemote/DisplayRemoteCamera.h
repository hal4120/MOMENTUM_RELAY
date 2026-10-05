#pragma once

#include "../CameraBase.h"

class DisplayRemoteCamera : public CameraBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="fixedLookAtPos">注視点（固定）</param>
	/// <param name="lookAtDiff">注視点からの相対座標</param>
	/// <param name="ROT_POWER">回転量</param>
	/// <param name="angle">初期角度</param>
	/// <param name="fov">視野角</param>
	DisplayRemoteCamera(
		const Vector3& fixedLookAtPos,
		const Vector3& lookAtDiff = Vector3::Zonly(-400),
		float ROT_POWER = 3.0f * (DX_PI_F / 180.0f),
		const Vector3& angle = Vector3(),
		float fov = 60.0f * (DX_PI_F / 180.0f)
	);

	// デストラクタ
	~DisplayRemoteCamera()override = default;

private:

	// 注視点（固定）
	const Vector3 fixedLookAtPos;

	// 注視点からの相対座標
	const Vector3 lookAtDiff;

	// 回転量
	const float ROT_POWER;

	// 操作角度
	Vector3 controlAngle;

	// 更新
	void NormalUpdate(void)override;

	// 終了
	void SubRelease(void)override;
};