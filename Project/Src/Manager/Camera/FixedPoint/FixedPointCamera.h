#pragma once

#include "../CameraBase.h"

class FixedPointCamera : public CameraBase
{
public:

	struct AngleTag {};
	struct LookAtTag {};

	static constexpr AngleTag Angle{};
	static constexpr LookAtTag LookAt{};

	/// <summary>
	/// カメラ情報（座標、角度、視野角）を直接指定する
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="angle">角度</param>
	/// <param name="fov">視野角</param>
	FixedPointCamera(AngleTag, const Vector3& pos, const Vector3& angle, float fov = 60.0f * (DX_PI_F / 180.0f)) : CameraBase(pos, angle, fov) {}

	/// <summary>
	/// カメラ座標と注視点を指定する
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="lookPos">注視点</param>
	/// <param name="fov">視野角</param>
	FixedPointCamera(LookAtTag, const Vector3& pos, const Vector3& lookPos, float fov = 60.0f * (DX_PI_F / 180.0f)) : CameraBase(pos, CalcCameraAngle(pos, lookPos), fov) {}


	// デストラクタ
	~FixedPointCamera()override = default;
};